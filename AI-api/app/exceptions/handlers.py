from fastapi import Request, status
from fastapi.responses import JSONResponse
from .custom_exceptions import (
    ModelLoadError,
    ImageProcessingError,
    PredictionError,
    InvalidImageFormatError,
    FileTooLargeError
)


async def model_load_error_handler(request: Request, exc: ModelLoadError):
    return JSONResponse(
        status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
        content={
            "success": False,
            "message": str(exc.message),
            "data": None,
            "error": "MODEL_LOAD_ERROR"
        }
    )


async def image_processing_error_handler(request: Request, exc: ImageProcessingError):
    return JSONResponse(
        status_code=status.HTTP_400_BAD_REQUEST,
        content={
            "success": False,
            "message": str(exc.message),
            "data": None,
            "error": "IMAGE_PROCESSING_ERROR"
        }
    )


async def prediction_error_handler(request: Request, exc: PredictionError):
    return JSONResponse(
        status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
        content={
            "success": False,
            "message": str(exc.message),
            "data": None,
            "error": "PREDICTION_ERROR"
        }
    )


async def invalid_image_format_error_handler(request: Request, exc: InvalidImageFormatError):
    return JSONResponse(
        status_code=status.HTTP_400_BAD_REQUEST,
        content={
            "success": False,
            "message": str(exc.message),
            "data": None,
            "error": "INVALID_IMAGE_FORMAT"
        }
    )


async def file_too_large_error_handler(request: Request, exc: FileTooLargeError):
    return JSONResponse(
        status_code=status.HTTP_413_REQUEST_ENTITY_TOO_LARGE,
        content={
            "success": False,
            "message": str(exc.message),
            "data": None,
            "error": "FILE_TOO_LARGE"
        }
    )


def register_exception_handlers(app):
    """Register all custom exception handlers to the FastAPI app"""
    app.add_exception_handler(ModelLoadError, model_load_error_handler)
    app.add_exception_handler(ImageProcessingError, image_processing_error_handler)
    app.add_exception_handler(PredictionError, prediction_error_handler)
    app.add_exception_handler(InvalidImageFormatError, invalid_image_format_error_handler)
    app.add_exception_handler(FileTooLargeError, file_too_large_error_handler)
