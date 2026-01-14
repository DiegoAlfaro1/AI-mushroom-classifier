# Mushroom Classifier API - Architecture Documentation

## 🏗️ Project Structure

```
AI-api/
├── app/
│   ├── __init__.py
│   ├── main.py                    # FastAPI application entry point
│   ├── core/                      # Core configuration
│   │   ├── __init__.py
│   │   └── config.py             # Centralized settings with pydantic-settings
│   ├── exceptions/                # Custom exceptions and handlers
│   │   ├── __init__.py
│   │   ├── custom_exceptions.py  # Custom exception classes
│   │   └── handlers.py           # FastAPI exception handlers
│   ├── schemas/                   # Pydantic schemas (API contracts)
│   │   ├── __init__.py
│   │   ├── base.py               # BaseResponse wrapper
│   │   └── prediction.py         # Prediction request/response schemas
│   ├── services/                  # Business logic layer
│   │   ├── __init__.py
│   │   ├── model_service.py      # ML model operations (singleton)
│   │   ├── prediction_service.py # Prediction orchestration
│   │   └── models/               # Model architectures
│   │       ├── __init__.py
│   │       └── baseline_cnn.py   # BaselineCNN architecture
│   └── api/                       # API routes
│       ├── __init__.py
│       ├── router.py             # Main API router
│       └── v1/                   # Version 1 endpoints
│           ├── __init__.py
│           ├── prediction_router.py  # /api/v1/predict
│           └── health_router.py      # /api/v1/health
├── infrastructure/
│   ├── database.py               # SQLAlchemy setup
│   ├── models/                   # Database models
│   │   ├── __init__.py
│   │   └── prediction.py
│   └── weights/                  # Model checkpoints
│       └── baseline_cnn_epoch10.pth
├── alembic/                      # Database migrations
├── requirements.txt
├── Dockerfile
└── docker-compose.yml
```

## 🎯 Architecture Features

### 1. **Clean Architecture with Separation of Concerns**

- **API Layer** (`app/api/`): HTTP request/response handling
- **Service Layer** (`app/services/`): Business logic and ML operations
- **Schema Layer** (`app/schemas/`): Data validation and contracts
- **Infrastructure Layer** (`infrastructure/`): Database and external resources

### 2. **API Versioning**

- Versioned routes under `/api/v1/`
- Easy to add v2 without breaking existing clients
- Clear upgrade path

### 3. **Custom Exception Handling**

- Typed exceptions for different error scenarios
- Standardized error responses
- Proper HTTP status codes

### 4. **OOP Design**

- `ModelService`: Singleton pattern for model management
- `PredictionService`: Service orchestration
- `BaselineCNN`: Reusable model architecture

### 5. **Configuration Management**

- `pydantic-settings` for type-safe configuration
- Environment variable support
- Centralized settings in `app/core/config.py`

### 6. **Async/Await Optimization**

- Async endpoints for better concurrency
- CPU-bound inference runs in thread pool executor
- Non-blocking I/O operations

## 📋 API Endpoints

### **Root**

- `GET /` - API welcome and information
- `GET /info` - Detailed API information

### **Health (v1)**

- `GET /api/v1/health/` - Health check with model status
- `GET /api/v1/health/model-info` - Detailed model information

### **Prediction (v1)**

- `POST /api/v1/predict/` - Upload image for prediction

  - **Parameters:**
    - `file`: Image file (multipart/form-data)
    - `top_k`: Number of predictions (1-10, default: 5)
    - `save_to_db`: Save to database (boolean, default: false)
  - **Returns:** Top K predictions with confidence scores

- `GET /api/v1/predict/test` - Test prediction endpoint

## 🚀 Usage

### Starting the API

```bash
# Install dependencies
pip install -r requirements.txt

# Run with uvicorn
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000

# Or with docker
docker-compose up
```

### Making Predictions

```bash
# Using curl
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5" \
  -F "save_to_db=false"

# Using Python requests
import requests

with open("mushroom.jpg", "rb") as f:
    files = {"file": f}
    data = {"top_k": 5, "save_to_db": False}
    response = requests.post(
        "http://localhost:8000/api/v1/predict/",
        files=files,
        data=data
    )
    print(response.json())
```

### Response Format

```json
{
  "success": true,
  "message": "Prediction successful",
  "data": {
    "top_prediction": "Amanita muscaria",
    "confidence": 0.95,
    "top_k_predictions": [
      { "species": "Amanita muscaria", "confidence": 0.95 },
      { "species": "Amanita pantherina", "confidence": 0.03 },
      { "species": "Amanita phalloides", "confidence": 0.01 }
    ],
    "timestamp": "2024-01-13T12:00:00"
  }
}
```

## 🔧 Configuration

Environment variables (`.env` file):

```env
# API Settings
API_VERSION=v1
PROJECT_NAME=Mushroom Classifier API
DEBUG=False

# Database
DATABASE_URL=postgresql://postgres:postgres@localhost:5432/mushroom_classifier

# Model
MODEL_PATH=./infrastructure/weights/baseline_cnn_epoch10.pth
IMG_SIZE=224

# Server
HOST=0.0.0.0
PORT=8000
WORKERS=1

# Upload
MAX_UPLOAD_SIZE=10485760  # 10MB in bytes
```

## 🧪 Testing

```bash
# Check API health
curl http://localhost:8000/api/v1/health/

# Check model info
curl http://localhost:8000/api/v1/health/model-info

# Interactive API docs
# Open in browser: http://localhost:8000/docs
```

## 📦 Model Loading

The API automatically loads the model at startup from the checkpoint. The checkpoint must contain:

- `model_state_dict`: Model weights
- `num_classes`: Number of classification classes
- `class_names`: List of species names
- `idx_to_class`: Mapping from index to class name

## 🔄 Adding New Features

### Adding a New Endpoint

1. Create router in `app/api/v1/new_router.py`
2. Include in `app/api/v1/__init__.py`
3. Define schemas in `app/schemas/`
4. Implement logic in `app/services/`

### Adding API v2

1. Create `app/api/v2/` directory
2. Create routers with v2 logic
3. Include in `app/api/router.py`

### Custom Exception

1. Define in `app/exceptions/custom_exceptions.py`
2. Create handler in `app/exceptions/handlers.py`
3. Register in `register_exception_handlers()`

## 🏆 Best Practices Implemented

- ✅ Dependency injection (FastAPI Depends)
- ✅ Singleton pattern for model service
- ✅ Async/await for I/O operations
- ✅ Thread pool for CPU-bound tasks
- ✅ Type hints throughout
- ✅ Pydantic validation
- ✅ Structured logging ready
- ✅ CORS middleware
- ✅ Database session management
- ✅ Lifespan events for startup/shutdown

## 📝 Further Improvements

Consider adding:

- **Rate limiting** - Protect against abuse
- **Authentication** - JWT or API keys
- **Caching** - Redis for predictions
- **Monitoring** - Prometheus metrics
- **Logging** - Structured logging (JSON)
- **Testing** - pytest unit/integration tests
- **CI/CD** - GitHub Actions pipeline

## 🐛 Troubleshooting

**Model not loading:**

- Check `MODEL_PATH` in config
- Verify checkpoint file exists
- Ensure checkpoint has required keys

**Import errors:**

- Install all requirements: `pip install -r requirements.txt`
- Check Python version (3.8+)

**Database connection:**

- Verify PostgreSQL is running
- Check `DATABASE_URL` configuration
- Run migrations: `alembic upgrade head`

## 📚 Technologies Used

- **FastAPI**: Modern web framework
- **PyTorch**: Deep learning inference
- **Pydantic**: Data validation
- **SQLAlchemy**: ORM for database
- **Alembic**: Database migrations
- **Uvicorn**: ASGI server
