from abc import ABC, abstractmethod


class Measurement(ABC):
    def __init__(self, timestamp):
        self.timestamp = timestamp
        super().__init__()

    @abstractmethod
    def to_dict():
        pass
