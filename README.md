# AI Mushroom Classifier

A deep learning-based mushroom species classifier using convolutional neural networks. This project includes a training pipeline, a FastAPI backend for inference, and a Qt desktop application.

## Project Components

| Component                           | Description                          |
| ----------------------------------- | ------------------------------------ |
| [AI-training](AI-training/)         | Jupyter notebooks for model training |
| [AI-api](AI-api/)                   | FastAPI backend for predictions      |
| [AI-mushroom-app](AI-mushroom-app/) | Qt6 desktop application              |

## Architecture Overview

```
AI-mushroom-classifier/
├── AI-training/          # Model training notebooks
│   ├── training.ipynb
│   ├── training_optimized.ipynb
│   └── weights/          # Trained model checkpoints
├── AI-api/               # FastAPI prediction service
│   ├── app/              # Application source code
│   ├── infrastructure/   # Database and model weights
│   ├── docs/             # API documentation
│   └── docker-compose.yml
└── AI-mushroom-app/      # Qt desktop client
    └── CMakeLists.txt
```

## Quick Start

### Start the API

```bash
cd AI-api
docker compose up -d --build
```

### Test a Prediction

```bash
curl -X POST "http://localhost:8000/api/v1/predict/" \
  -F "file=@mushroom.jpg" \
  -F "top_k=5"
```

## Documentation

- [API Getting Started](AI-api/docs/getting-started.md)
- [API Reference](AI-api/docs/api-reference.md)
- [API Architecture](AI-api/docs/architecture.md)

## Technology Stack

- **Deep Learning**: PyTorch, CNN
- **Backend**: FastAPI, PostgreSQL, SQLAlchemy
- **Desktop App**: Qt6, C++
- **Containerization**: Docker
