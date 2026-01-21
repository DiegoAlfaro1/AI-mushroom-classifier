# Getting Started

This guide covers the installation and initial setup of the Mushroom Classifier API.

## Prerequisites

### For Docker Deployment (Recommended)

- Docker
- Docker Compose
- Trained model file (.pth format)

### For Local Development

- Python 3.8+
- PostgreSQL (optional, for database features)
- Trained model file (.pth format)

## Installation

### Option 1: Docker Deployment

1. **Configure Environment Variables**

   The `.env` file should contain:

   ```env
   POSTGRES_USER=postgres
   POSTGRES_PASSWORD=your_password
   POSTGRES_DB=mushroom_classifier
   POSTGRES_PORT=5432
   DATABASE_URL=postgresql+psycopg2://postgres:your_password@db:5432/mushroom_classifier
   API_PORT=8000
   MODEL_PATH=/app/weights/model.pth
   ```

2. **Add Model Weights**

   Place your trained PyTorch model in the `infrastructure/weights/` directory:

   ```bash
   cp /path/to/your/model.pth infrastructure/weights/baseline_cnn_epoch10.pth
   ```

3. **Start the API**

   ```bash
   docker compose up -d --build
   ```

   This command will:

   - Build the Docker images
   - Start PostgreSQL database with health checks
   - Start the FastAPI application
   - Run pending migrations automatically

4. **Verify Installation**

   ```bash
   # Check container status
   docker compose ps

   # Test health endpoint
   curl http://localhost:8000/health
   ```

### Option 2: Local Development

1. **Install Dependencies**

   ```bash
   cd AI-api
   pip install -r requirements.txt
   ```

2. **Configure Environment**

   Create a `.env` file:

   ```env
   API_VERSION=v1
   PROJECT_NAME=Mushroom Classifier API
   DEBUG=True
   DATABASE_URL=postgresql://postgres:postgres@localhost:5432/mushroom_classifier
   MODEL_PATH=./infrastructure/weights/baseline_cnn_epoch10.pth
   ```

3. **Start the Server**

   ```bash
   # Development mode with auto-reload
   uvicorn app.main:app --reload --host 0.0.0.0 --port 8000

   # Production mode
   uvicorn app.main:app --workers 4 --host 0.0.0.0 --port 8000
   ```

## Quick Test

### Health Check

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

### Make a Prediction

```bash
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5"
```

### Interactive Documentation

Access the API documentation in your browser:

- **Swagger UI**: http://localhost:8000/docs
- **ReDoc**: http://localhost:8000/redoc

## Database Setup

### Initial Migration (First Time Only)

```bash
# Generate migration from models
docker compose exec api alembic revision --autogenerate -m "Initial migration"

# Apply the migration
docker compose exec api alembic upgrade head
```

### Database Schema

The `predictions` table stores:

| Column     | Type         | Description                              |
| ---------- | ------------ | ---------------------------------------- |
| id         | Integer (PK) | Unique identifier                        |
| image_path | Text         | Path or reference to the predicted image |
| prediction | String       | The classification result                |
| created_at | DateTime     | Timestamp when prediction was made       |

## Next Steps

- Review the [API Reference](api-reference.md) for complete endpoint documentation
- See [Configuration](configuration.md) for all available options
- Check [Architecture](architecture.md) to understand the system design
