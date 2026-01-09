# Mushroom Classifier API

FastAPI-based API for mushroom classification using a PyTorch model (.pth format) with PostgreSQL database.

## Prerequisites

- Docker
- Docker Compose
- Your trained model file (.pth format)

## Quick Start

### 1. Configure Environment Variables

The `.env` file is already configured. If you need to change database credentials or ports, edit the `.env` file:

```env
POSTGRES_USER=postgres
POSTGRES_PASSWORD=your_password
POSTGRES_DB=mushroom_classifier
POSTGRES_PORT=5432
DATABASE_URL=postgresql+psycopg2://postgres:your_password@db:5432/mushroom_classifier
API_PORT=8000
MODEL_PATH=/app/weights/model.pth
```

### 2. Add Your Model Weights

Place your trained PyTorch model (.pth file) in the `weights/` directory:

```bash
# The directory already exists
cp /path/to/your/model.pth weights/model.pth
```

### 3. Start the API

Build and start all services (PostgreSQL database + FastAPI):

```bash
docker compose up -d --build
```

This command will:
- Build the Docker images
- Start PostgreSQL database with health checks
- Start the FastAPI application
- Run any pending migrations automatically

### 4. Verify the API is Running

Check if the containers are running:

```bash
docker compose ps
```

Test the API health endpoint:

```bash
curl http://localhost:8000/health
```

You should see: `{"status":"healthy"}`

### 5. Create Initial Database Migration

If this is your first time running the API, create the initial migration:

```bash
# Generate migration from your models
docker compose exec api alembic revision --autogenerate -m "Initial migration"

# Apply the migration
docker compose exec api alembic upgrade head
```

## Database Schema

The `predictions` table stores:

- `id` (Integer, Primary Key): Unique identifier
- `image_path` (Text): Path or reference to the predicted image
- `prediction` (String): The classification result
- `created_at` (DateTime): Timestamp when prediction was made

## API Documentation

Once the API is running, access the interactive documentation:

- **Swagger UI**: http://localhost:8000/docs
- **ReDoc**: http://localhost:8000/redoc
- **Root endpoint**: http://localhost:8000/

## Common Commands

### Managing the API

```bash
# Start the API
docker compose up -d

# Stop the API
docker compose down

# Restart the API
docker compose restart api

# View logs (all services)
docker compose logs -f

# View logs (API only)
docker compose logs -f api

# View logs (Database only)
docker compose logs -f db

# Rebuild and restart after code changes
docker compose up -d --build
```

### Database Operations

```bash
# Access PostgreSQL shell
docker compose exec db psql -U postgres -d mushroom_classifier

# Create a new migration after changing models
docker compose exec api alembic revision --autogenerate -m "Description of changes"

# Apply pending migrations
docker compose exec api alembic upgrade head

# View migration history
docker compose exec api alembic history

# Rollback last migration
docker compose exec api alembic downgrade -1
```

### Cleanup Commands

```bash
# Stop and remove containers (keeps database data)
docker compose down

# Stop, remove containers, and delete database data
docker compose down -v

# Remove all images
docker compose down --rmi all
```

## Development

The project is configured for development with:

- **Hot Reload**: Code changes in `app/` automatically restart the server
- **Volume Mounting**: Local files are mounted into containers for instant updates
- **Auto-migrations**: Pending migrations run automatically on startup

### Making Code Changes

1. Edit files in the `app/` directory
2. Changes will be automatically detected and the server will reload
3. No need to rebuild containers for code changes

### Adding New Dependencies

1. Add the package to `requirements.txt`
2. Rebuild the container:
   ```bash
   docker compose up -d --build
   ```

## Project Structure

```
AI-api/
├── app/
│   ├── models/
│   │   ├── __init__.py
│   │   └── prediction.py      # SQLAlchemy models
│   ├── database.py             # Database configuration
│   ├── main.py                 # FastAPI application
│   └── __init__.py
├── alembic/                    # Database migrations
│   ├── versions/               # Migration files
│   ├── env.py                  # Alembic configuration
│   └── script.py.mako
├── weights/                    # PyTorch model files
├── Dockerfile                  # API container definition
├── docker-compose.yml          # Service orchestration
├── requirements.txt            # Python dependencies
├── alembic.ini                 # Alembic settings
├── .env                        # Environment variables
└── README.md
```

## Troubleshooting

### "Host db not known" Error

This error occurs when the container can't resolve the database hostname. Solutions:

1. Ensure both containers are on the same network (already configured)
2. Check that `.env` file is being loaded correctly
3. Verify DATABASE_URL uses the correct format: `postgresql+psycopg2://user:pass@db:5432/dbname`
4. Restart the containers:
   ```bash
   docker compose down
   docker compose up -d
   ```

### Port Already in Use

If ports 8000 or 5432 are already in use, modify `.env`:

```env
API_PORT=8001
POSTGRES_PORT=5433
```

Then rebuild:
```bash
docker compose down
docker compose up -d --build
```

### Database Connection Refused

Check if the database is healthy:

```bash
docker compose ps
```

The db container should show "healthy" status. If not, check logs:

```bash
docker compose logs db
```

### Migration Errors

If you encounter migration issues:

```bash
# Check current migration status
docker compose exec api alembic current

# View migration history
docker compose exec api alembic history

# Reset database (WARNING: Deletes all data)
docker compose down -v
docker compose up -d
```

### Container Won't Start

Check logs for errors:

```bash
docker compose logs api
```

Common issues:
- Missing model file in `weights/` directory
- Incorrect DATABASE_URL format
- Port conflicts

### Viewing Real-time Logs

```bash
# Follow logs for all services
docker compose logs -f

# Follow logs for specific service
docker compose logs -f api
```
