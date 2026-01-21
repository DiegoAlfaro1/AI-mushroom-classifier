# Mushroom Classifier API

FastAPI-based REST API for mushroom species classification using a PyTorch convolutional neural network.

## Features

- RESTful API with versioning (v1)
- Image classification with confidence scores
- Top-K predictions support
- PostgreSQL database integration
- Docker containerization
- Interactive API documentation (Swagger UI / ReDoc)

## Quick Start

### Using Docker (Recommended)

```bash
# Start all services
docker compose up -d --build

# Verify the API is running
curl http://localhost:8000/api/v1/health/
```

### Local Development

```bash
# Install dependencies
pip install -r requirements.txt

# Start the server
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

## Making Predictions

```bash
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5"
```

## API Documentation

When the server is running:

- **Swagger UI**: http://localhost:8000/docs
- **ReDoc**: http://localhost:8000/redoc

## Project Documentation

Complete documentation is available in the [docs](docs/) folder:

| Document                                   | Description            |
| ------------------------------------------ | ---------------------- |
| [Getting Started](docs/getting-started.md) | Installation and setup |
| [API Reference](docs/api-reference.md)     | Endpoint documentation |
| [Architecture](docs/architecture.md)       | System design          |
| [Configuration](docs/configuration.md)     | Environment variables  |
| [Troubleshooting](docs/troubleshooting.md) | Common issues          |

## Project Structure

```
AI-api/
├── app/
│   ├── main.py           # Application entry point
│   ├── core/             # Configuration
│   ├── api/              # API routes (v1)
│   ├── schemas/          # Pydantic models
│   ├── services/         # Business logic
│   └── exceptions/       # Error handling
├── infrastructure/
│   ├── database.py       # SQLAlchemy setup
│   ├── models/           # Database models
│   └── weights/          # Model checkpoints
├── alembic/              # Database migrations
├── docs/                 # Documentation
├── Dockerfile
├── docker-compose.yml
└── requirements.txt
```

## Technology Stack

- **Framework**: FastAPI
- **ML Library**: PyTorch
- **Database**: PostgreSQL + SQLAlchemy
- **Migrations**: Alembic
- **Server**: Uvicorn
- **Containerization**: Docker
