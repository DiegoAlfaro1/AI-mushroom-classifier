# Troubleshooting

Solutions for common issues with the Mushroom Classifier API.

## Docker Issues

### "Host db not known" Error

**Problem**: Container cannot resolve the database hostname.

**Solutions**:

1. Ensure both containers are on the same Docker network
2. Verify `.env` file is being loaded correctly
3. Check DATABASE_URL format: `postgresql+psycopg2://user:pass@db:5432/dbname`
4. Restart containers:

   ```bash
   docker compose down
   docker compose up -d
   ```

### Container Won't Start

**Problem**: Container fails to start or exits immediately.

**Diagnosis**:

```bash
docker compose logs api
```

**Common causes**:

- Missing model file in `infrastructure/weights/`
- Incorrect DATABASE_URL format
- Port conflicts

### Port Already in Use

**Problem**: Ports 8000 or 5432 are already in use.

**Solution**: Modify `.env` file:

```env
API_PORT=8001
POSTGRES_PORT=5433
```

Then rebuild:

```bash
docker compose down
docker compose up -d --build
```

## Database Issues

### Connection Refused

**Problem**: Cannot connect to the database.

**Diagnosis**:

```bash
docker compose ps
```

The db container should show "healthy" status. If not:

```bash
docker compose logs db
```

### Migration Errors

**Problem**: Alembic migration fails.

**Solutions**:

```bash
# Check current migration status
docker compose exec api alembic current

# View migration history
docker compose exec api alembic history

# Reset database (WARNING: Deletes all data)
docker compose down -v
docker compose up -d
```

## Model Issues

### Model Not Loading

**Problem**: API starts but predictions fail with "Model not loaded".

**Diagnosis**:

1. Verify checkpoint exists:

   ```bash
   ls infrastructure/weights/
   ```

2. Check MODEL_PATH configuration in `.env`

3. Review startup logs:

   ```bash
   docker compose logs api | grep -i model
   ```

**Solutions**:

- Ensure the checkpoint file is in the correct location
- Verify file permissions
- Check that the checkpoint contains required keys:
  - `model_state_dict`
  - `num_classes`
  - `class_names`
  - `idx_to_class`

### Architecture Mismatch

**Problem**: Error loading model weights.

**Cause**: The model architecture in code does not match the saved checkpoint.

**Solution**: Ensure `app/services/models/baseline_cnn.py` matches the architecture used during training.

## API Issues

### Import Errors

**Problem**: `ModuleNotFoundError` when starting the API.

**Solution**:

```bash
pip install -r requirements.txt
```

### CORS Errors

**Problem**: Browser requests are blocked with CORS errors.

**Solution**: Update `ALLOWED_ORIGINS` in `app/core/config.py`:

```python
ALLOWED_ORIGINS = [
    "http://localhost",
    "http://localhost:3000",
    "http://your-frontend-domain.com",
]
```

### File Too Large

**Problem**: 413 error when uploading images.

**Solution**: Increase `MAX_UPLOAD_SIZE` in `.env`:

```env
MAX_UPLOAD_SIZE=20971520  # 20 MB
```

### Invalid Image Format

**Problem**: 400 error with "Invalid image format".

**Supported formats**:

- `.jpg`, `.jpeg`
- `.png`
- `.webp`

**Solution**: Convert the image to a supported format before uploading.

## Performance Issues

### Slow Predictions

**Problem**: Predictions take too long.

**Solutions**:

1. Ensure model is running on GPU (if available)
2. Reduce image size before uploading
3. Increase worker count for production:

   ```bash
   uvicorn app.main:app --workers 4
   ```

### High Memory Usage

**Problem**: Container uses too much memory.

**Solutions**:

1. Limit container memory in `docker-compose.yml`:

   ```yaml
   services:
     api:
       deploy:
         resources:
           limits:
             memory: 2G
   ```

2. Reduce batch size or model size

## Logging

### Viewing Real-time Logs

```bash
# All services
docker compose logs -f

# Specific service
docker compose logs -f api
```

### Checking Startup Logs

```bash
docker compose logs api | head -50
```

## Quick Diagnostic Commands

```bash
# Check container status
docker compose ps

# Check API health
curl http://localhost:8000/api/v1/health/

# Check model status
curl http://localhost:8000/api/v1/health/model-info

# View OpenAPI schema
curl http://localhost:8000/openapi.json | head -100
```

## Getting Help

1. Check the logs for detailed error messages
2. Review the [Architecture](architecture.md) documentation
3. Inspect the OpenAPI schema at `/openapi.json`
4. Test individual endpoints in the Swagger UI at `/docs`
