from pydantic import BaseModel, Field
from typing import List
from datetime import datetime


class TopKPrediction(BaseModel):
    species: str = Field(..., description="Mushroom species name")
    confidence: float = Field(..., ge=0.0, le=1.0, description="Confidence score (0-1)")
    
    class Config:
        json_schema_extra = {
            "example": {
                "species": "Amanita muscaria",
                "confidence": 0.95
            }
        }


class PredictionRequest(BaseModel):
    top_k: int = Field(default=5, ge=1, le=10, description="Number of top predictions to return")
    
    class Config:
        json_schema_extra = {
            "example": {
                "top_k": 5
            }
        }


class PredictionResponse(BaseModel):
    top_prediction: str = Field(..., description="Most likely species")
    confidence: float = Field(..., ge=0.0, le=1.0, description="Confidence of top prediction")
    top_k_predictions: List[TopKPrediction] = Field(..., description="Top K predictions")
    timestamp: datetime = Field(default_factory=datetime.utcnow, description="Prediction timestamp")
    
    class Config:
        json_schema_extra = {
            "example": {
                "top_prediction": "Amanita muscaria",
                "confidence": 0.95,
                "top_k_predictions": [
                    {"species": "Amanita muscaria", "confidence": 0.95},
                    {"species": "Amanita pantherina", "confidence": 0.03}
                ],
                "timestamp": "2024-01-13T12:00:00"
            }
        }


class HealthResponse(BaseModel):
    status: str = Field(..., description="Service status")
    model_loaded: bool = Field(..., description="Whether model is loaded")
    num_classes: int = Field(..., description="Number of classes the model can predict")
    
    class Config:
        json_schema_extra = {
            "example": {
                "status": "healthy",
                "model_loaded": True,
                "num_classes": 182
            }
        }
