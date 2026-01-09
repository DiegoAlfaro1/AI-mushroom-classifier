from fastapi import FastAPI
from app.database import engine, Base

# Create database tables
Base.metadata.create_all(bind=engine)

app = FastAPI(
    title="Mushroom Classifier API",
    description="API for classifying mushroom images",
    version="1.0.0"
)


@app.get("/")
async def root():
    return {"message": "Mushroom Classifier API is running"}


@app.get("/health")
async def health_check():
    return {"status": "healthy"}
