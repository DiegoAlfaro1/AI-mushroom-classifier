# Architecture

This document describes the system architecture, project structure, and design patterns used in the Mushroom Classifier API.

## Project Structure

```
AI-api/
├── app/
│   ├── __init__.py
│   ├── main.py                    # FastAPI application entry point
│   ├── core/
│   │   ├── __init__.py
│   │   └── config.py              # Centralized settings (pydantic-settings)
│   ├── exceptions/
│   │   ├── __init__.py
│   │   ├── custom_exceptions.py   # Custom exception classes
│   │   └── handlers.py            # FastAPI exception handlers
│   ├── schemas/
│   │   ├── __init__.py
│   │   ├── base.py                # BaseResponse wrapper
│   │   └── prediction.py          # Prediction request/response schemas
│   ├── services/
│   │   ├── __init__.py
│   │   ├── model_service.py       # ML model operations (singleton)
│   │   ├── prediction_service.py  # Prediction orchestration
│   │   └── models/
│   │       ├── __init__.py
│   │       └── baseline_cnn.py    # BaselineCNN architecture
│   └── api/
│       ├── __init__.py
│       ├── router.py              # Main API router
│       └── v1/
│           ├── __init__.py
│           ├── prediction_router.py
│           └── health_router.py
├── infrastructure/
│   ├── database.py                # SQLAlchemy setup
│   ├── models/
│   │   ├── __init__.py
│   │   └── prediction.py          # Database models
│   └── weights/
│       └── baseline_cnn_epoch10.pth
├── alembic/                       # Database migrations
├── docs/                          # Documentation
├── requirements.txt
├── Dockerfile
└── docker-compose.yml
```

## Architecture Layers

The application follows a clean architecture with clear separation of concerns:

```
+----------------------------------------------------------+
|                       API Layer                           |
|  +-----------------------------------------------------+  |
|  |  /api/v1/predict/ (POST)  - Image prediction        |  |
|  |  /api/v1/health/  (GET)   - Health check            |  |
|  +-----------------------------------------------------+  |
+----------------------------------------------------------+
                            |
                            v
+----------------------------------------------------------+
|                     Service Layer                         |
|  +-----------------------------------------------------+  |
|  |  PredictionService - Orchestration and DB operations|  |
|  |  ModelService - ML inference (Singleton)            |  |
|  +-----------------------------------------------------+  |
+----------------------------------------------------------+
                            |
                            v
+----------------------------------------------------------+
|                   Infrastructure Layer                    |
|  +-----------------------------------------------------+  |
|  |  Database (SQLAlchemy) - Prediction storage         |  |
|  |  BaselineCNN Model - PyTorch inference              |  |
|  +-----------------------------------------------------+  |
+----------------------------------------------------------+
```

### Layer Responsibilities

| Layer          | Directory         | Responsibility                          |
| -------------- | ----------------- | --------------------------------------- |
| API            | `app/api/`        | HTTP request/response handling, routing |
| Service        | `app/services/`   | Business logic, ML operations           |
| Schema         | `app/schemas/`    | Data validation and API contracts       |
| Infrastructure | `infrastructure/` | Database, external resources            |
| Core           | `app/core/`       | Configuration, settings                 |
| Exceptions     | `app/exceptions/` | Error handling                          |

## Design Patterns

### Singleton Pattern

`ModelService` uses the singleton pattern to ensure only one model instance is loaded in memory:

```python
class ModelService:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance
```

### Service Layer Pattern

Business logic is encapsulated in service classes:

- `ModelService`: Handles model loading, preprocessing, and inference
- `PredictionService`: Orchestrates predictions and database operations

### Dependency Injection

FastAPI's `Depends` is used for dependency injection:

```python
@router.post("/")
async def predict(
    file: UploadFile,
    prediction_service: PredictionService = Depends()
):
    ...
```

## API Versioning

Routes are organized under `/api/v1/` for easy versioning:

```python
# app/api/router.py
api_router = APIRouter(prefix="/api")
api_router.include_router(v1_router, prefix="/v1")
```

Adding a new version:

1. Create `app/api/v2/` directory
2. Implement v2 routers
3. Include in `app/api/router.py` with `/v2` prefix

## Async Processing

The API uses async/await for optimal concurrency:

- **Async Endpoints**: Non-blocking request handling
- **ThreadPoolExecutor**: CPU-bound inference runs in a thread pool
- **Non-blocking I/O**: Database and file operations

```python
async def predict(self, image_bytes: bytes) -> dict:
    loop = asyncio.get_event_loop()
    result = await loop.run_in_executor(
        self._executor,
        self._run_inference,
        image_bytes
    )
    return result
```

## Model Loading

The model is loaded at application startup using FastAPI lifespan events:

```python
@asynccontextmanager
async def lifespan(app: FastAPI):
    # Startup
    model_service = ModelService()
    model_service.load_model()
    yield
    # Shutdown
    pass
```

### Checkpoint Requirements

The model checkpoint must contain:

| Key              | Type | Description                      |
| ---------------- | ---- | -------------------------------- |
| model_state_dict | dict | Model weights                    |
| num_classes      | int  | Number of classification classes |
| class_names      | list | List of species names            |
| idx_to_class     | dict | Index to class name mapping      |

## Image Preprocessing

Images are preprocessed before inference:

1. Resize to 224x224 pixels
2. Convert to RGB
3. Normalize with ImageNet statistics:
   - Mean: [0.485, 0.456, 0.406]
   - Std: [0.229, 0.224, 0.225]
4. Add batch dimension

## Exception Handling

Custom exceptions provide typed error handling:

| Exception               | HTTP Status | Use Case                  |
| ----------------------- | ----------- | ------------------------- |
| ModelLoadError          | 503         | Model loading failures    |
| ImageProcessingError    | 400         | Image processing failures |
| PredictionError         | 500         | Prediction failures       |
| InvalidImageFormatError | 400         | Invalid image format      |
| FileTooLargeError       | 413         | File size exceeded        |

## Extending the API

### Adding a New Endpoint

1. Create router in `app/api/v1/new_router.py`
2. Define schemas in `app/schemas/`
3. Implement logic in `app/services/`
4. Include router in `app/api/v1/__init__.py`

### Adding a Custom Exception

1. Define in `app/exceptions/custom_exceptions.py`
2. Create handler in `app/exceptions/handlers.py`
3. Register in `register_exception_handlers()`
