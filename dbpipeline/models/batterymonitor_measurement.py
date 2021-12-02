from datetime import datetime

from .measurement import Measurement


class BatterymonintorMeasurement(Measurement):
    def __init__(self, timestamp, battery1, battery2):
        self.timestamp = timestamp
        self.battery1 = battery1
        self.battery2 = battery2

    @classmethod
    def from_shadow(cls, shadow):
        reported = shadow['state']['reported']
        battery1 = reported['battery1']
        battery2 = reported['battery2']
        timestamp = datetime.utcfromtimestamp(
            int(shadow['metadata']['reported']['battery1']['voltage']['timestamp']))

        return cls(
            timestamp=timestamp,
            battery1=battery1,
            battery2=battery2
        )

    def to_dict(self):
        return {'timestamp': self.timestamp, 'battery1': self.battery1,
                'battery2': self.battery2}
