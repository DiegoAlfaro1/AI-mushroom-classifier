from sqlalchemy import Column, Integer, String, DateTime, Text
from sqlalchemy.sql import func
from infrastructure.database import Base


class Prediction(Base):
    __tablename__ = "predictions"

    id = Column(Integer, primary_key=True, index=True)
    image_path = Column(Text, nullable=False)
    prediction = Column(String(255), nullable=False)
    created_at = Column(DateTime(timezone=True), server_default=func.now())

    def __repr__(self):
        return f"<Prediction(id={self.id}, prediction='{self.prediction}')>"
