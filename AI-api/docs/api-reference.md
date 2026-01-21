# API Reference

Complete documentation for all available API endpoints.

## Base URL

```
http://localhost:8000
```

## Endpoints Overview

| Method | Endpoint                    | Description              |
| ------ | --------------------------- | ------------------------ |
| GET    | `/`                         | API welcome message      |
| GET    | `/info`                     | API information          |
| GET    | `/docs`                     | Swagger UI documentation |
| GET    | `/redoc`                    | ReDoc documentation      |
| GET    | `/api/v1/health/`           | Health check             |
| GET    | `/api/v1/health/model-info` | Model information        |
| POST   | `/api/v1/predict/`          | Make prediction          |
| GET    | `/api/v1/predict/test`      | Test prediction endpoint |

---

## Root Endpoints

### GET /

Returns a welcome message and basic API information.

**Response:**

```json
{
  "message": "Welcome to Mushroom Classifier API",
  "version": "v1"
}
```

### GET /info

Returns detailed API information.

---

## Health Endpoints

### GET /api/v1/health/

Check API health status and model availability.

**Response:**

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

### GET /api/v1/health/model-info

Returns detailed information about the loaded model.

**Response:**

```json
{
  "success": true,
  "message": "Model information retrieved",
  "data": {
    "model_loaded": true,
    "num_classes": 182,
    "class_names": ["species1", "species2", "..."]
  }
}
```

---

## Prediction Endpoints

### POST /api/v1/predict/

Upload an image for mushroom species classification.

**Request:**

- **Content-Type**: `multipart/form-data`

**Parameters:**

| Parameter  | Type    | Required | Default | Description                                |
| ---------- | ------- | -------- | ------- | ------------------------------------------ |
| file       | File    | Yes      | -       | Image file (JPG, PNG, WEBP)                |
| top_k      | Integer | No       | 5       | Number of predictions to return (1-10)     |
| save_to_db | Boolean | No       | false   | Whether to save the prediction to database |

**Supported Image Formats:**

- `.jpg`, `.jpeg`
- `.png`
- `.webp`

**File Size Limit:** 10 MB (configurable)

**Example Request (cURL):**

```bash
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5" \
  -F "save_to_db=false"
```

**Example Request (Python):**

```python
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

**Success Response (200 OK):**

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

**Error Responses:**

| Status Code | Error                   | Description                 |
| ----------- | ----------------------- | --------------------------- |
| 400         | InvalidImageFormatError | Unsupported image format    |
| 400         | ImageProcessingError    | Failed to process the image |
| 413         | FileTooLargeError       | File exceeds size limit     |
| 500         | PredictionError         | Internal prediction error   |
| 503         | ModelLoadError          | Model not loaded            |

### GET /api/v1/predict/test

Test endpoint to verify the prediction service is operational.

---

## Response Format

All API responses follow a standardized format:

```json
{
  "success": true,
  "message": "Operation description",
  "data": {
    // Response-specific data
  }
}
```

### Error Response Format

```json
{
  "success": false,
  "message": "Error description",
  "data": null
}
```

---

## Python Client Example

```python
import requests
from pathlib import Path

API_URL = "http://localhost:8000/api/v1/predict/"

image_path = Path("mushroom.jpg")

with open(image_path, "rb") as f:
    files = {"file": f}
    data = {"top_k": 5, "save_to_db": False}

    response = requests.post(API_URL, files=files, data=data)
    result = response.json()

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
