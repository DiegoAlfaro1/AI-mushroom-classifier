from pydantic import BaseModel
from typing import Optional, Any


class BaseResponse(BaseModel):
    success: bool
    message: str
    data: Optional[Any] = None
    
    class Config:
        json_schema_extra = {
            "example": {
                "success": True,
                "message": "Operation successful",
                "data": {}
            }
        }
