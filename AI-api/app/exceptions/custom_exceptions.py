class ModelLoadError(Exception):
    """Raised when model fails to load"""
    def __init__(self, message: str = "Failed to load model"):
        self.message = message
        super().__init__(self.message)


class ImageProcessingError(Exception):
    """Raised when image processing fails"""
    def __init__(self, message: str = "Failed to process image"):
        self.message = message
        super().__init__(self.message)


class PredictionError(Exception):
    """Raised when prediction fails"""
    def __init__(self, message: str = "Prediction failed"):
        self.message = message
        super().__init__(self.message)


class InvalidImageFormatError(Exception):
    """Raised when image format is not supported"""
    def __init__(self, message: str = "Invalid image format"):
        self.message = message
        super().__init__(self.message)


class FileTooLargeError(Exception):
    """Raised when uploaded file exceeds size limit"""
    def __init__(self, message: str = "File too large"):
        self.message = message
        super().__init__(self.message)
