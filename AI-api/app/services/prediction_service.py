from datetime import datetime
from typing import Optional, Dict, Any
from PIL import Image
from sqlalchemy.orm import Session

from app.services.model_service import ModelService
from app.exceptions import PredictionError
from infrastructure.models.prediction import Prediction


class PredictionService:
    """
    Service layer for orchestrating predictions and database operations.
    Coordinates between ModelService and database.
    """
    
    def __init__(self, model_service: ModelService):
        self.model_service = model_service
    
    async def predict_and_save(
        self,
        image: Image.Image,
        image_path: str,
        top_k: int,
        db: Optional[Session] = None
    ) -> Dict[str, Any]:
        """
        Run prediction and optionally save to database.
        
        Args:
            image: PIL Image to predict
            image_path: Path/identifier for the image
            top_k: Number of top predictions
            db: Database session (optional)
            
        Returns:
            Prediction results with timestamp
        """
        try:
            # Get prediction from model service
            prediction_result = await self.model_service.predict(image, top_k=top_k)
            
            # Add timestamp
            prediction_result["timestamp"] = datetime.utcnow()
            
            # Save to database if session provided
            if db is not None:
                db_prediction = Prediction(
                    image_path=image_path,
                    prediction=prediction_result["top_prediction"]
                )
                db.add(db_prediction)
                db.commit()
                db.refresh(db_prediction)
                prediction_result["id"] = db_prediction.id
            
            return prediction_result
            
        except Exception as e:
            if db is not None:
                db.rollback()
            raise PredictionError(f"Prediction failed: {str(e)}")
    
    async def predict(self, image: Image.Image, top_k: int = 5) -> Dict[str, Any]:
        """
        Run prediction without database operation.
        
        Args:
            image: PIL Image to predict
            top_k: Number of top predictions
            
        Returns:
            Prediction results with timestamp
        """
        try:
            prediction_result = await self.model_service.predict(image, top_k=top_k)
            prediction_result["timestamp"] = datetime.utcnow()
            return prediction_result
        except Exception as e:
            raise PredictionError(f"Prediction failed: {str(e)}")
