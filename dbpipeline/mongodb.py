import certifi
import pymongo
from bson.objectid import ObjectId


class MongoDB:
    def __init__(self, connection_string, db, collection):
        self.CA = certifi.where()
        self.client = pymongo.MongoClient(
            connection_string, tlsCAFile=self.CA)
        self.collection = self.client[db][collection]

    def find_one(self, query):
        return self.collection.find_one(query)

    def insert_one(self, object):
        return self.collection.insert_one(object)

    def find_one_and_update_by_id(self, id, query):
        return self.collection.find_one_and_update({'_id': ObjectId(id)}, query)
