import torch
import torch.nn as nn
from torchvision import transforms
from PIL import Image
from pathlib import Path
from typing import Dict, Any, Optional, Tuple
import asyncio
from concurrent.futures import ThreadPoolExecutor

from app.core.config import settings
from app.exceptions import ModelLoadError, ImageProcessingError
from app.services.models import BaselineCNN


class ModelService:
    """
    Service class for ML model operations.
    Handles model loading, preprocessing, and inference.
    Uses singleton pattern for efficient resource usage.
    """
    
    _instance: Optional["ModelService"] = None
    _executor: ThreadPoolExecutor = ThreadPoolExecutor(max_workers=2)
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance
    
    def __init__(self):
        if self._initialized:
            return
            
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.model: Optional[nn.Module] = None
        self.class_names: list[str] = []
        self.idx_to_class: Dict[int, str] = {}
        self.num_classes: int = 0
        self.img_size: int = settings.IMG_SIZE
        
        # Preprocessing transform
        self.transform = transforms.Compose([
            transforms.Resize((self.img_size, self.img_size)),
            transforms.ToTensor(),
            transforms.Normalize(
                mean=settings.NORMALIZE_MEAN,
                std=settings.NORMALIZE_STD
            )
        ])
        
        self._initialized = True
    
    def load_model(self, model_path: Optional[str] = None) -> None:
        """
        Load the PyTorch model from checkpoint.
        
        Args:
            model_path: Path to model checkpoint. Uses settings.MODEL_PATH if None.
            
        Raises:
            ModelLoadError: If model loading fails
        """
        try:
            model_path = model_path or settings.MODEL_PATH
            checkpoint_path = Path(model_path)
            
            if not checkpoint_path.exists():
                raise ModelLoadError(f"Model file not found: {model_path}")
            
            # Load checkpoint
            checkpoint = torch.load(
                checkpoint_path,
                map_location=self.device,
                weights_only=False
            )
            
            # Extract model metadata
            self.num_classes = checkpoint.get("num_classes")
            self.class_names = checkpoint.get("class_names", [])
            self.idx_to_class = checkpoint.get("idx_to_class", {})
            
            # Convert string keys to int for idx_to_class
            if self.idx_to_class and isinstance(list(self.idx_to_class.keys())[0], str):
                self.idx_to_class = {int(k): v for k, v in self.idx_to_class.items()}
            
            if not self.num_classes:
                raise ModelLoadError("num_classes not found in checkpoint")
            
            # Create model architecture
            self.model = BaselineCNN(num_classes=self.num_classes)
            
            # Load state dict
            state_dict = checkpoint.get("model_state_dict")
            if state_dict is None:
                raise ModelLoadError("model_state_dict not found in checkpoint")
            
            self.model.load_state_dict(state_dict)
            self.model.to(self.device)
            self.model.eval()
            
            print(f"✓ Model loaded successfully")
            print(f"  - Device: {self.device}")
            print(f"  - Classes: {self.num_classes}")
            print(f"  - Model type: BaselineCNN")
            
        except Exception as e:
            raise ModelLoadError(f"Failed to load model: {str(e)}")
    
    def preprocess_image(self, image: Image.Image) -> torch.Tensor:
        """
        Preprocess image for model input.
        
        Args:
            image: PIL Image
            
        Returns:
            Preprocessed tensor
            
        Raises:
            ImageProcessingError: If preprocessing fails
        """
        try:
            if image.mode != "RGB":
                image = image.convert("RGB")
            
            tensor = self.transform(image)
            tensor = tensor.unsqueeze(0)  # Add batch dimension
            
            return tensor
            
        except Exception as e:
            raise ImageProcessingError(f"Failed to preprocess image: {str(e)}")
    
    def _predict_sync(self, image_tensor: torch.Tensor, top_k: int = 5) -> Tuple[list, list]:
        """
        Synchronous prediction (runs in thread pool).
        
        Args:
            image_tensor: Preprocessed image tensor
            top_k: Number of top predictions to return
            
        Returns:
            Tuple of (species_list, confidence_list)
        """
        if self.model is None:
            raise ModelLoadError("Model not loaded. Call load_model() first.")
        
        with torch.no_grad():
            image_tensor = image_tensor.to(self.device)
            outputs = self.model(image_tensor)
            probabilities = torch.softmax(outputs, dim=1)
            
            top_probs, top_indices = torch.topk(probabilities, k=min(top_k, self.num_classes))
            
            top_probs = top_probs[0].cpu().numpy().tolist()
            top_indices = top_indices[0].cpu().numpy().tolist()
            
            species = [self.idx_to_class.get(idx, f"Unknown_{idx}") for idx in top_indices]
            
        return species, top_probs
    
    async def predict(self, image: Image.Image, top_k: int = 5) -> Dict[str, Any]:
        """
        Async prediction using thread pool executor.
        
        Args:
            image: PIL Image
            top_k: Number of top predictions to return
            
        Returns:
            Dictionary containing prediction results
        """
        # Preprocess in main thread (fast operation)
        image_tensor = self.preprocess_image(image)
        
        # Run inference in thread pool (CPU-bound operation)
        loop = asyncio.get_event_loop()
        species, confidences = await loop.run_in_executor(
            self._executor,
            self._predict_sync,
            image_tensor,
            top_k
        )
        
        return {
            "top_prediction": species[0],
            "confidence": confidences[0],
            "top_k_predictions": [
                {"species": sp, "confidence": conf}
                for sp, conf in zip(species, confidences)
            ]
        }
    
    def is_loaded(self) -> bool:
        """Check if model is loaded and ready"""
        return self.model is not None
    
    def get_model_info(self) -> Dict[str, Any]:
        """Get model information"""
        return {
            "loaded": self.is_loaded(),
            "num_classes": self.num_classes,
            "device": str(self.device),
            "class_names": self.class_names[:10] if len(self.class_names) > 10 else self.class_names
        }
