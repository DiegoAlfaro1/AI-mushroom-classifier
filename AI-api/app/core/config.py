from pydantic_settings import BaseSettings
from pathlib import Path
from typing import Optional


class Settings(BaseSettings):
    # API Settings
    API_VERSION: str = "v1"
    PROJECT_NAME: str = "Mushroom Classifier API"
    DEBUG: bool = False
    
    # Database
    DATABASE_URL: str = "postgresql://postgres:postgres@localhost:5432/mushroom_classifier"
    
    # Model Settings
    MODEL_PATH: str = str(Path(__file__).parent.parent.parent / "infrastructure" / "weights" / "baseline_cnn_epoch10.pth")
    IMG_SIZE: int = 224
    NORMALIZE_MEAN: list[float] = [0.485, 0.456, 0.406]
    NORMALIZE_STD: list[float] = [0.229, 0.224, 0.225]
    
    # Server Settings
    HOST: str = "0.0.0.0"
    PORT: int = 8000
    WORKERS: int = 1
    
    # CORS
    ALLOWED_ORIGINS: list[str] = ["*"]
    
    # Upload Settings
    MAX_UPLOAD_SIZE: int = 10 * 1024 * 1024  # 10MB
    ALLOWED_EXTENSIONS: set[str] = {".jpg", ".jpeg", ".png", ".webp"}
    
    class Config:
        env_file = ".env"
        case_sensitive = True


settings = Settings()
