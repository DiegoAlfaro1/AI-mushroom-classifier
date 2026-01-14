from fastapi import APIRouter
from .prediction_router import router as prediction_router
from .health_router import router as health_router

# Create v1 API router
api_v1_router = APIRouter(prefix="/v1")

# Include all v1 routers
api_v1_router.include_router(prediction_router)
api_v1_router.include_router(health_router)

__all__ = ["api_v1_router"]
