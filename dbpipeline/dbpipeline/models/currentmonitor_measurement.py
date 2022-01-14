from .measurement import Measurement
from datetime import datetime


class CurrentmonitorMeasurement(Measurement):
    def __init__(self, timestamp, generated, consumed):
        self.timestamp = timestamp
        self.generated = generated
        self.consumed = consumed

    @classmethod
    def from_shadow(cls, shadow):
        reported = shadow['state']['reported']
        generated = reported['generated']
        consumed = reported['consumed']
        timestamp = datetime.utcfromtimestamp(
            int(shadow['metadata']['reported']['generated']['generatedCurrent']['timestamp']))

        return cls(
            timestamp=timestamp,
            generated=generated,
            consumed=consumed
        )

    def to_dict(self):
        return {'timestamp': self.timestamp, 'generated': self.generated,
                'consumed': self.consumed}
