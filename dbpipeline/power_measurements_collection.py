from datetime import datetime, timedelta


class PowerMeasurementsCollection:
    def __init__(self, mdb_collection):
        self.sensor_id = 'batterymonitor'
        self.mdb_collection = mdb_collection

    def insert_measurement(self, measurement):
        raw_array_object = self.__build_raw_array_object(measurement.timestamp)
        parent_object = self.__is_array_object_existing(raw_array_object)
        parent_object_id = parent_object['_id'] if parent_object is not None else None
        if not parent_object:
            parent_object_id = self.__insert_raw_array_object(
                raw_array_object).inserted_id

        update_query = {'$addToSet': {'measurements': measurement.to_dict()}}
        return self.mdb_collection.find_one_and_update_by_id(parent_object_id, update_query)

    def __build_raw_array_object(self, timestamp):
        start_time = self.__convert_reported_timestamp_to_last_ten_minute(
            timestamp)
        end_time = start_time + timedelta(minutes=10)
        return {'sensor_id':  self.sensor_id, 'start_time': start_time,
                'end_time': end_time, 'measurements': []}

    def __is_array_object_existing(self, raw_array_object):
        query = {'sensor_id': self.sensor_id,
                 'start_time': raw_array_object['start_time']}
        return self.mdb_collection.find_one(query)

    def __insert_raw_array_object(self, raw_array_object):
        return self.mdb_collection.insert_one(raw_array_object)

    def __convert_reported_timestamp_to_last_ten_minute(self, timestamp):
        this_hour = timestamp.replace(minute=0, second=0)
        minutes_diff = int((timestamp - this_hour).total_seconds() / 60.0)
        nearest_ten = round(minutes_diff/10)*10
        if(nearest_ten < minutes_diff):
            return timestamp.replace(minute=nearest_ten, second=0)
        return timestamp.replace(minute=nearest_ten-10, second=0)
