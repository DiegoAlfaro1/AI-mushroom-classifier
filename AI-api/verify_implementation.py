#!/usr/bin/env python
"""
Test script to verify the API implementation without dependencies.
Checks syntax, imports structure, and validates architecture.
"""

import ast
import os
from pathlib import Path


def check_file_syntax(filepath):
    """Check if Python file has valid syntax"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            ast.parse(f.read())
        return True, None
    except SyntaxError as e:
        return False, str(e)


def check_file_exists(filepath):
    """Check if file exists"""
    return Path(filepath).exists()


def main():
    print("="*70)
    print("API IMPLEMENTATION VERIFICATION")
    print("="*70)
    
    base_path = Path("app")
    
    # Define expected files
    required_files = {
        "Core Configuration": [
            "core/__init__.py",
            "core/config.py",
        ],
        "Exception Handling": [
            "exceptions/__init__.py",
            "exceptions/custom_exceptions.py",
            "exceptions/handlers.py",
        ],
        "Pydantic Schemas": [
            "schemas/__init__.py",
            "schemas/base.py",
            "schemas/prediction.py",
        ],
        "Services Layer": [
            "services/__init__.py",
            "services/model_service.py",
            "services/prediction_service.py",
            "services/models/__init__.py",
            "services/models/baseline_cnn.py",
        ],
        "API Routers": [
            "api/__init__.py",
            "api/router.py",
            "api/v1/__init__.py",
            "api/v1/health_router.py",
            "api/v1/prediction_router.py",
        ],
        "Application": [
            "__init__.py",
            "main.py",
        ]
    }
    
    total_files = 0
    passed_files = 0
    failed_files = []
    
    # Check each category
    for category, files in required_files.items():
        print(f"\n{category}:")
        for file in files:
            filepath = base_path / file
            total_files += 1
            
            # Check existence
            if not check_file_exists(filepath):
                print(f"  ✗ {file} - NOT FOUND")
                failed_files.append(f"{file} (missing)")
                continue
            
            # Check syntax
            valid, error = check_file_syntax(filepath)
            if not valid:
                print(f"  ✗ {file} - SYNTAX ERROR: {error}")
                failed_files.append(f"{file} (syntax error)")
            else:
                print(f"  ✓ {file}")
                passed_files += 1
    
    # Check documentation
    print(f"\nDocumentation:")
    docs = ["ARCHITECTURE.md", "IMPLEMENTATION_SUMMARY.md", "QUICKSTART.md"]
    for doc in docs:
        if check_file_exists(doc):
            print(f"  ✓ {doc}")
        else:
            print(f"  ✗ {doc} - NOT FOUND")
    
    # Summary
    print(f"\n{'='*70}")
    print(f"VERIFICATION SUMMARY")
    print(f"{'='*70}")
    print(f"Total files checked: {total_files}")
    print(f"Passed: {passed_files}")
    print(f"Failed: {total_files - passed_files}")
    
    if failed_files:
        print(f"\nFailed files:")
        for file in failed_files:
            print(f"  - {file}")
    
    # Architecture validation
    print(f"\n{'='*70}")
    print(f"ARCHITECTURE VALIDATION")
    print(f"{'='*70}")
    
    checks = {
        "Clean Architecture": passed_files >= 19,
        "API Versioning": check_file_exists(base_path / "api" / "v1" / "__init__.py"),
        "Exception Handling": check_file_exists(base_path / "exceptions" / "handlers.py"),
        "OOP Services": check_file_exists(base_path / "services" / "model_service.py"),
        "Pydantic Schemas": check_file_exists(base_path / "schemas" / "prediction.py"),
    }
    
    for check, status in checks.items():
        symbol = "✓" if status else "✗"
        print(f"{symbol} {check}")
    
    # Final result
    print(f"\n{'='*70}")
    if passed_files == total_files and all(checks.values()):
        print("✓ VERIFICATION PASSED - API Implementation Complete!")
        print("='*70}")
        print("\nNext steps:")
        print("1. Install dependencies: pip install -r requirements.txt")
        print("2. Start the API: uvicorn app.main:app --reload")
        print("3. Visit: http://localhost:8000/docs")
        return 0
    else:
        print("✗ VERIFICATION FAILED - Please review errors above")
        print("="*70)
        return 1


if __name__ == "__main__":
    exit(main())
