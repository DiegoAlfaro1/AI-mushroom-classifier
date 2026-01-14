from fastapi import APIRouter, UploadFile, File, Form, Depends, HTTPException, status
from sqlalchemy.orm import Session
from PIL import Image
import io
from pathlib import Path

from app.schemas import BaseResponse, PredictionResponse
from app.services import ModelService, PredictionService
from app.exceptions import InvalidImageFormatError, FileTooLargeError, ImageProcessingError
from app.core.config import settings
from infrastructure.database import get_db


router = APIRouter(prefix="/predict", tags=["Prediction"])

# Singleton instances
model_service = ModelService()
prediction_service = PredictionService(model_service)


def validate_image_file(file: UploadFile) -> None:
    """Validate uploaded image file"""
    # Check file extension
    file_ext = Path(file.filename).suffix.lower()
    if file_ext not in settings.ALLOWED_EXTENSIONS:
        raise InvalidImageFormatError(
            f"Invalid file format. Allowed: {', '.join(settings.ALLOWED_EXTENSIONS)}"
        )


async def load_image_from_upload(file: UploadFile) -> Image.Image:
    """Load PIL Image from uploaded file"""
    try:
        # Read file content
        contents = await file.read()
        
        # Check file size
        if len(contents) > settings.MAX_UPLOAD_SIZE:
            raise FileTooLargeError(
                f"File too large. Max size: {settings.MAX_UPLOAD_SIZE / (1024*1024):.1f}MB"
            )
        
        # Load image
        image = Image.open(io.BytesIO(contents))
        
        # Validate image can be opened
        image.verify()
        
        # Reload image after verify (verify closes it)
        image = Image.open(io.BytesIO(contents))
        
        return image
        
    except (InvalidImageFormatError, FileTooLargeError):
        raise
    except Exception as e:
        raise ImageProcessingError(f"Failed to load image: {str(e)}")


@router.post(
    "/",
    response_model=BaseResponse,
    status_code=status.HTTP_200_OK,
    summary="Predict mushroom species",
    description="Upload an image to predict the mushroom species. Returns top K predictions with confidence scores."
)
async def predict_mushroom(
    file: UploadFile = File(..., description="Image file (JPG, PNG, WEBP)"),
    top_k: int = Form(default=5, ge=1, le=10, description="Number of top predictions to return"),
    save_to_db: bool = Form(default=False, description="Save prediction to database"),
    db: Session = Depends(get_db)
):
    """
    Predict mushroom species from uploaded image.
    
    - **file**: Image file to classify (required)
    - **top_k**: Number of top predictions (1-10, default 5)
    - **save_to_db**: Whether to save prediction to database
    
    Returns prediction results with confidence scores.
    """
    # Validate file
    validate_image_file(file)
    
    # Load image
    image = await load_image_from_upload(file)
    
    # Run prediction
    if save_to_db:
        result = await prediction_service.predict_and_save(
            image=image,
            image_path=file.filename,
            top_k=top_k,
            db=db
        )
    else:
        result = await prediction_service.predict(
            image=image,
            top_k=top_k
        )
    
    # Format response
    response_data = PredictionResponse(**result)
    
    return BaseResponse(
        success=True,
        message="Prediction successful",
        data=response_data.model_dump()
    )


@router.get(
    "/test",
    response_model=BaseResponse,
    summary="Test prediction endpoint",
    description="Simple endpoint to test if prediction service is working"
)
async def test_prediction():
    """Test endpoint to check if prediction service is accessible"""
    return BaseResponse(
        success=True,
        message="Prediction endpoint is working",
        data={
            "model_loaded": model_service.is_loaded(),
            "num_classes": model_service.num_classes
        }
    )
