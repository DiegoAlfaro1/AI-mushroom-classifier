from fastapi import APIRouter, status
from app.schemas import BaseResponse, HealthResponse
from app.services import ModelService

router = APIRouter(prefix="/health", tags=["Health"])

# Singleton instance
model_service = ModelService()


@router.get(
    "/",
    response_model=BaseResponse,
    status_code=status.HTTP_200_OK,
    summary="Health check",
    description="Check if the API and model are operational"
)
async def health_check():
    """
    Health check endpoint.
    
    Returns service status and model information.
    """
    model_info = model_service.get_model_info()
    
    health_data = HealthResponse(
        status="healthy" if model_info["loaded"] else "degraded",
        model_loaded=model_info["loaded"],
        num_classes=model_info["num_classes"]
    )
    
    return BaseResponse(
        success=True,
        message="Service is operational",
        data=health_data.model_dump()
    )


@router.get(
    "/model-info",
    response_model=BaseResponse,
    status_code=status.HTTP_200_OK,
    summary="Get model information",
    description="Get detailed information about the loaded model"
)
async def get_model_info():
    """
    Get detailed model information.
    
    Returns model configuration and loaded state.
    """
    model_info = model_service.get_model_info()
    
    return BaseResponse(
        success=True,
        message="Model information retrieved",
        data=model_info
    )
