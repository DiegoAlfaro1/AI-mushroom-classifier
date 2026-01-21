# Mushroom Classifier API Documentation

This directory contains comprehensive documentation for the Mushroom Classifier API.

## Documentation Index

| Document                              | Description                                           |
| ------------------------------------- | ----------------------------------------------------- |
| [Getting Started](getting-started.md) | Installation, prerequisites, and quick start guide    |
| [API Reference](api-reference.md)     | Complete endpoint documentation and usage examples    |
| [Architecture](architecture.md)       | System design, project structure, and design patterns |
| [Configuration](configuration.md)     | Environment variables and configuration options       |
| [Troubleshooting](troubleshooting.md) | Common issues and solutions                           |

## Overview

The Mushroom Classifier API is a FastAPI-based REST API that provides mushroom species classification using a trained PyTorch convolutional neural network. The API accepts image uploads and returns predictions with confidence scores.

### Key Features

- RESTful API with versioning (v1)
- Image classification with top-K predictions
- PostgreSQL database integration for prediction storage
- Docker containerization support
- Interactive API documentation (Swagger UI and ReDoc)
- Async processing for optimal performance

### Technology Stack

- **Framework**: FastAPI
- **ML Library**: PyTorch
- **Database**: PostgreSQL with SQLAlchemy ORM
- **Migrations**: Alembic
- **Server**: Uvicorn (ASGI)
- **Containerization**: Docker and Docker Compose
