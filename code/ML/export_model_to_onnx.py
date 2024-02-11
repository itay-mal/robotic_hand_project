import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import torch.utils.data as data



# TODO: write in seperate file and import both here and in notebook to avoid code duplication
class MotionClassifier(nn.Module):

    def __init__(self, num_tags=5, num_features=6, num_hidden=500, num_layers=1):
        super().__init__()
        self.num_tags = num_tags
        self.lstm = nn.LSTM(input_size=num_features, hidden_size=num_hidden, num_layers=num_layers, batch_first=True, dtype=torch.float32)

        # The linear layer that maps from hidden state space to tag space
        self.linear = nn.Linear(num_hidden, num_tags, dtype=torch.float32)
        self.logSoftMax = nn.LogSoftmax(dim=2)

    def forward(self, x):
        x, _ = self.lstm(x)
        x = self.linear(x)
        x = self.logSoftMax(x)
        return x

# load pretrained params
model = MotionClassifier()
model.load_state_dict(torch.load('code/ML/trained_model.pt'))
model.eval()

# save as onnx
torch_input = torch.randn(1, 1, 32, 32)
onnx_program = torch.onnx.export(model, torch_input)

onnx_program.save("my_motion_classifier.onnx")