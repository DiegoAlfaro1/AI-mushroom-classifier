from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager

from app.core.config import settings
from app.api import api_router
from app.exceptions import register_exception_handlers
from app.services import ModelService
from infrastructure.database import engine, Base


@asynccontextmanager
async def lifespan(app: FastAPI):
    """
    Lifespan event handler for startup and shutdown.
    Initializes ModelService singleton at startup.
    """
    # Startup: Load model
    print(f"\n{'='*70}")
    print(f"Starting {settings.PROJECT_NAME}")
    print(f"{'='*70}")
    
    model_service = ModelService()
    try:
        model_service.load_model()
        print(f"✓ Model loaded successfully")
    except Exception as e:
        print(f"⚠ Warning: Failed to load model - {str(e)}")
        print(f"  API will start but predictions will fail until model is loaded")
    
    # Create database tables
    Base.metadata.create_all(bind=engine)
    print(f"✓ Database tables created/verified")
    print(f"{'='*70}\n")
    
    yield
    
    # Shutdown
    print(f"\n{'='*70}")
    print(f"Shutting down {settings.PROJECT_NAME}")
    print(f"{'='*70}\n")


# Create FastAPI app
app = FastAPI(
    title=settings.PROJECT_NAME,
    description="Production-ready API for mushroom species classification using CNN",
    version=settings.API_VERSION,
    lifespan=lifespan,
    docs_url="/docs",
    redoc_url="/redoc",
    openapi_url="/openapi.json"
)

# Configure CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.ALLOWED_ORIGINS,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Register exception handlers
register_exception_handlers(app)

# Include API routers
app.include_router(api_router)


@app.get("/", tags=["Root"])
async def root():
    """Root endpoint - API information"""
    return {
        "message": f"Welcome to {settings.PROJECT_NAME}",
        "version": settings.API_VERSION,
        "docs": "/docs",
        "health": "/api/v1/health"
    }


@app.get("/info", tags=["Root"])
async def info():
    """Get API information"""
    return {
        "name": settings.PROJECT_NAME,
        "version": settings.API_VERSION,
        "debug": settings.DEBUG,
        "endpoints": {
            "docs": "/docs",
            "health": "/api/v1/health",
            "predict": "/api/v1/predict"
        }
    }
