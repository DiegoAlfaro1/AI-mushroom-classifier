"""
Script to export PyTorch BaselineCNN model to ONNX format.

Usage:
    python export_to_onnx.py --checkpoint baseline_cnn_best.pth --output baseline_cnn.onnx
"""

import argparse
import torch
import torch.nn as nn

# python export_to_onnx.py --checkpoint baseline_cnn_best.pth --output baseline_cnn.onnx


class BaselineCNN(nn.Module):
    """
    Baseline CNN - must match the architecture used during training.
    """
    def __init__(self, num_classes):
        super(BaselineCNN, self).__init__()

        self.features = nn.Sequential(
            # Block 1: 224 -> 112
            nn.Conv2d(3, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2, 2),

            # Block 2: 112 -> 56
            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2, 2),

            # Block 3: 56 -> 28
            nn.Conv2d(64, 128, kernel_size=3, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2, 2),

            # Block 4: 28 -> 14
            nn.Conv2d(128, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2, 2),

            # Block 5: 14 -> 7
            nn.Conv2d(256, 512, kernel_size=3, padding=1),
            nn.BatchNorm2d(512),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2, 2),
        )

        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))

        self.classifier = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(512, 256),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(256, num_classes)
        )

    def forward(self, x):
        x = self.features(x)
        x = self.avgpool(x)
        x = torch.flatten(x, 1)
        x = self.classifier(x)
        return x


def load_checkpoint(checkpoint_path, device='cpu'):
    """Load checkpoint and extract model info."""
    checkpoint = torch.load(checkpoint_path, map_location=device)

    num_classes = checkpoint['num_classes']
    class_names = checkpoint.get('class_names', [])
    img_size = checkpoint.get('img_size', 224)

    model = BaselineCNN(num_classes=num_classes)
    model.load_state_dict(checkpoint['model_state_dict'])
    model.eval()

    return model, {
        'num_classes': num_classes,
        'class_names': class_names,
        'img_size': img_size,
        'normalize_mean': checkpoint.get('normalize_mean', [0.485, 0.456, 0.406]),
        'normalize_std': checkpoint.get('normalize_std', [0.229, 0.224, 0.225]),
    }


def export_to_onnx(model, output_path, img_size=224, opset_version=17):
    """Export PyTorch model to ONNX format."""
    model.eval()

    # Create dummy input with correct shape: (batch, channels, height, width)
    dummy_input = torch.randn(1, 3, img_size, img_size)

    # Define input/output names for clarity
    input_names = ['input']
    output_names = ['output']

    # Dynamic axes allow variable batch size
    dynamic_axes = {
        'input': {0: 'batch_size'},
        'output': {0: 'batch_size'}
    }

    # Export to ONNX
    torch.onnx.export(
        model,
        dummy_input,
        output_path,
        export_params=True,
        opset_version=opset_version,
        do_constant_folding=True,
        input_names=input_names,
        output_names=output_names,
        dynamic_axes=dynamic_axes,
    )

    print(f"Model exported to: {output_path}")


def verify_onnx(onnx_path, img_size=224):
    """Verify the exported ONNX model."""
    try:
        import onnx
        import onnxruntime as ort
        import numpy as np

        # Check model is valid
        onnx_model = onnx.load(onnx_path)
        onnx.checker.check_model(onnx_model)
        print("ONNX model structure is valid.")

        # Test inference with ONNX Runtime
        session = ort.InferenceSession(onnx_path)

        # Get input details
        input_name = session.get_inputs()[0].name
        input_shape = session.get_inputs()[0].shape
        print(f"Input name: {input_name}")
        print(f"Input shape: {input_shape}")

        # Get output details
        output_name = session.get_outputs()[0].name
        output_shape = session.get_outputs()[0].shape
        print(f"Output name: {output_name}")
        print(f"Output shape: {output_shape}")

        # Run test inference
        test_input = np.random.randn(1, 3, img_size, img_size).astype(np.float32)
        result = session.run([output_name], {input_name: test_input})
        print(f"Test inference successful. Output shape: {result[0].shape}")

        return True

    except ImportError as e:
        print(f"Skipping verification (missing dependency): {e}")
        print("Install with: pip install onnx onnxruntime")
        return False


def main():
    parser = argparse.ArgumentParser(
        description='Export BaselineCNN PyTorch model to ONNX format'
    )
    parser.add_argument(
        '--checkpoint', '-c',
        type=str,
        required=True,
        help='Path to the .pth checkpoint file'
    )
    parser.add_argument(
        '--output', '-o',
        type=str,
        default=None,
        help='Output ONNX file path (default: same name as checkpoint with .onnx extension)'
    )
    parser.add_argument(
        '--opset',
        type=int,
        default=17,
        help='ONNX opset version (default: 17)'
    )
    parser.add_argument(
        '--no-verify',
        action='store_true',
        help='Skip ONNX verification step'
    )

    args = parser.parse_args()

    # Determine output path
    if args.output is None:
        args.output = args.checkpoint.replace('.pth', '.onnx')

    print("=" * 60)
    print("PyTorch to ONNX Export")
    print("=" * 60)
    print(f"Checkpoint: {args.checkpoint}")
    print(f"Output: {args.output}")
    print(f"Opset version: {args.opset}")
    print("=" * 60)

    # Load model
    print("\nLoading checkpoint...")
    model, metadata = load_checkpoint(args.checkpoint)

    print(f"Number of classes: {metadata['num_classes']}")
    print(f"Image size: {metadata['img_size']}")
    if metadata['class_names']:
        print(f"First 5 classes: {metadata['class_names'][:5]}")

    # Export
    print("\nExporting to ONNX...")
    export_to_onnx(
        model,
        args.output,
        img_size=metadata['img_size'],
        opset_version=args.opset
    )

    # Verify
    if not args.no_verify:
        print("\nVerifying ONNX model...")
        verify_onnx(args.output, img_size=metadata['img_size'])

    print("\n" + "=" * 60)
    print("Export complete!")
    print("=" * 60)


if __name__ == '__main__':
    main()
