# Configuration

This document covers all configuration options available for the Mushroom Classifier API.

## Environment Variables

Configuration is managed through environment variables, loaded via `pydantic-settings`.

### API Settings

| Variable     | Type    | Default                 | Description                    |
| ------------ | ------- | ----------------------- | ------------------------------ |
| API_VERSION  | string  | v1                      | API version identifier         |
| PROJECT_NAME | string  | Mushroom Classifier API | Project name for documentation |
| DEBUG        | boolean | False                   | Enable debug mode              |

### Database Settings

| Variable          | Type    | Default             | Description                  |
| ----------------- | ------- | ------------------- | ---------------------------- |
| DATABASE_URL      | string  | -                   | PostgreSQL connection string |
| POSTGRES_USER     | string  | postgres            | Database user                |
| POSTGRES_PASSWORD | string  | -                   | Database password            |
| POSTGRES_DB       | string  | mushroom_classifier | Database name                |
| POSTGRES_PORT     | integer | 5432                | Database port                |

### Model Settings

| Variable   | Type    | Default                                           | Description              |
| ---------- | ------- | ------------------------------------------------- | ------------------------ |
| MODEL_PATH | string  | ./infrastructure/weights/baseline_cnn_epoch10.pth | Path to model checkpoint |
| IMG_SIZE   | integer | 224                                               | Image preprocessing size |

### Server Settings

| Variable | Type    | Default | Description                |
| -------- | ------- | ------- | -------------------------- |
| HOST     | string  | 0.0.0.0 | Server bind address        |
| PORT     | integer | 8000    | Server port                |
| WORKERS  | integer | 1       | Number of worker processes |
| API_PORT | integer | 8000    | API external port (Docker) |

### Upload Settings

| Variable        | Type    | Default  | Description                        |
| --------------- | ------- | -------- | ---------------------------------- |
| MAX_UPLOAD_SIZE | integer | 10485760 | Maximum file size in bytes (10 MB) |

## Configuration Files

### .env File (Local Development)

```env
# API Settings
API_VERSION=v1
PROJECT_NAME=Mushroom Classifier API
DEBUG=True

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
MAX_UPLOAD_SIZE=10485760
```

### .env File (Docker Deployment)

```env
# Database
POSTGRES_USER=postgres
POSTGRES_PASSWORD=your_secure_password
POSTGRES_DB=mushroom_classifier
POSTGRES_PORT=5432
DATABASE_URL=postgresql+psycopg2://postgres:your_secure_password@db:5432/mushroom_classifier

# API
API_PORT=8000
MODEL_PATH=/app/weights/model.pth
```

## Docker Configuration

### docker-compose.yml Services

| Service | Port | Description         |
| ------- | ---- | ------------------- |
| api     | 8000 | FastAPI application |
| db      | 5432 | PostgreSQL database |

### Volume Mounts

| Mount                                  | Purpose              |
| -------------------------------------- | -------------------- |
| ./app:/app/app                         | Live code reload     |
| ./infrastructure/weights:/app/weights  | Model weights        |
| postgres_data:/var/lib/postgresql/data | Database persistence |

## CORS Configuration

CORS settings are configured in `app/core/config.py`:

```python
ALLOWED_ORIGINS = [
    "http://localhost",
    "http://localhost:3000",
    "http://localhost:8080",
]
```

To add additional origins, modify the `ALLOWED_ORIGINS` list.

## Alembic Configuration

Database migrations are configured in `alembic.ini`:

```ini
[alembic]
script_location = alembic
sqlalchemy.url = %(DATABASE_URL)s
```

## Managing Docker Containers

### Starting Services

```bash
# Start all services
docker compose up -d

# Start with rebuild
docker compose up -d --build
```

### Stopping Services

```bash
# Stop containers (keep data)
docker compose down

# Stop and remove data volumes
docker compose down -v
```

### Viewing Logs

```bash
# All services
docker compose logs -f

# API only
docker compose logs -f api

# Database only
docker compose logs -f db
```

### Database Operations

```bash
# Access PostgreSQL shell
docker compose exec db psql -U postgres -d mushroom_classifier

# Create migration
docker compose exec api alembic revision --autogenerate -m "Description"

# Apply migrations
docker compose exec api alembic upgrade head

# View migration history
docker compose exec api alembic history

# Rollback last migration
docker compose exec api alembic downgrade -1
```

## Adding Dependencies

1. Add the package to `requirements.txt`
2. Rebuild the container:

   ```bash
   docker compose up -d --build
   ```

## Port Conflicts

If default ports are in use, modify the `.env` file:

```env
API_PORT=8001
POSTGRES_PORT=5433
```

Then restart:

```bash
docker compose down
docker compose up -d --build
```
