# 🚀 Quick Start Guide - Mushroom Classifier API

## Prerequisites

- Python 3.8+
- PostgreSQL (optional, for database features)
- Model checkpoint: `baseline_cnn_epoch10.pth` in `infrastructure/weights/`

## Installation

### 1. Install Dependencies

```bash
cd AI-api
pip install -r requirements.txt
```

### 2. Configure Environment (Optional)

Create `.env` file:

```env
API_VERSION=v1
PROJECT_NAME=Mushroom Classifier API
DEBUG=False
DATABASE_URL=postgresql://postgres:postgres@localhost:5432/mushroom_classifier
MODEL_PATH=./infrastructure/weights/baseline_cnn_epoch10.pth
```

### 3. Start the API

```bash
# Development mode (with auto-reload)
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000

# Production mode
uvicorn app.main:app --workers 4 --host 0.0.0.0 --port 8000
```

Server starts at: `http://localhost:8000`

## 🧪 Test the API

### 1. Check Health

```bash
curl http://localhost:8000/api/v1/health/
```

Expected response:

```json
{
  "success": true,
  "message": "Service is operational",
  "data": {
    "status": "healthy",
    "model_loaded": true,
    "num_classes": 182
  }
}
```

### 2. Make a Prediction

```bash
# Replace 'mushroom.jpg' with your image file
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5"
```

Expected response:

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
      { "species": "Amanita phalloides", "confidence": 0.01 },
      { "species": "Agaricus bisporus", "confidence": 0.005 },
      { "species": "Boletus edulis", "confidence": 0.003 }
    ],
    "timestamp": "2024-01-13T12:00:00"
  }
}
```

### 3. Interactive Documentation

Open in browser:

- **Swagger UI**: http://localhost:8000/docs
- **ReDoc**: http://localhost:8000/redoc

## 📝 Python Client Example

```python
import requests
from pathlib import Path

# API endpoint
API_URL = "http://localhost:8000/api/v1/predict/"

# Prepare image file
image_path = Path("mushroom.jpg")

# Make prediction
with open(image_path, "rb") as f:
    files = {"file": f}
    data = {"top_k": 5, "save_to_db": False}

    response = requests.post(API_URL, files=files, data=data)
    result = response.json()

# Print results
if result["success"]:
    prediction = result["data"]
    print(f"Top Prediction: {prediction['top_prediction']}")
    print(f"Confidence: {prediction['confidence']:.2%}")
    print("\nTop 5 Predictions:")
    for pred in prediction["top_k_predictions"]:
        print(f"  {pred['species']}: {pred['confidence']:.2%}")
else:
    print(f"Error: {result['message']}")
```

## 🐳 Docker Quick Start

```bash
# Build and run with Docker Compose
docker-compose up --build

# API available at http://localhost:8000
```

## 🔧 Configuration Options

### Request Parameters

- `file` (required): Image file (JPG, PNG, WEBP)
- `top_k` (optional): Number of predictions (1-10, default: 5)
- `save_to_db` (optional): Save to database (boolean, default: false)

### Supported Image Formats

- `.jpg`, `.jpeg`
- `.png`
- `.webp`

### File Size Limit

- Default: 10 MB
- Configure in `app/core/config.py`: `MAX_UPLOAD_SIZE`

## 📊 Available Endpoints

| Method | Endpoint                    | Description              |
| ------ | --------------------------- | ------------------------ |
| GET    | `/`                         | API welcome message      |
| GET    | `/info`                     | API information          |
| GET    | `/docs`                     | Swagger UI documentation |
| GET    | `/api/v1/health/`           | Health check             |
| GET    | `/api/v1/health/model-info` | Model information        |
| POST   | `/api/v1/predict/`          | Make prediction          |
| GET    | `/api/v1/predict/test`      | Test prediction endpoint |

## 🐛 Troubleshooting

### Model Not Loading

**Problem:** API starts but predictions fail
**Solution:**

- Verify model checkpoint exists at `infrastructure/weights/baseline_cnn_epoch10.pth`
- Check file permissions
- Review startup logs for errors

### Import Errors

**Problem:** `ModuleNotFoundError`
**Solution:**

```bash
pip install -r requirements.txt
```

### Database Connection Error

**Problem:** Database connection fails
**Solution:**

- Ensure PostgreSQL is running
- Update `DATABASE_URL` in `.env`
- Or set `save_to_db=false` in requests

### CORS Issues

**Problem:** Browser requests blocked
**Solution:** Update `ALLOWED_ORIGINS` in `app/core/config.py`

## 🎯 Next Steps

1. **Test with your images** - Try different mushroom species
2. **Integrate with frontend** - Use the API from your web/mobile app
3. **Monitor performance** - Check response times and accuracy
4. **Add features** - Authentication, rate limiting, caching
5. **Deploy** - AWS, GCP, Azure, or Heroku

## 📚 Documentation Links

- **Architecture**: See `ARCHITECTURE.md`
- **Implementation Summary**: See `IMPLEMENTATION_SUMMARY.md`
- **API Docs**: http://localhost:8000/docs (when running)

## 🆘 Need Help?

- Check the logs for detailed error messages
- Review the architecture documentation
- Inspect the OpenAPI schema at `/openapi.json`
- Test individual components in the interactive docs

---

**Ready to classify mushrooms! 🍄**
