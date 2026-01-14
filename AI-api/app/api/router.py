from fastapi import APIRouter
from .v1 import api_v1_router

# Main API router
api_router = APIRouter(prefix="/api")

# Include versioned routers
api_router.include_router(api_v1_router)

__all__ = ["api_router"]
