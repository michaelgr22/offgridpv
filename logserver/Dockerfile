FROM python:3.7.8

# Creating Application Source Code Directory
RUN mkdir -p /logserver/src

# Copying src code to Container
COPY logserver /logserver/src

# Setting Home Directory for containers
WORKDIR /logserver/src

CMD [ "python", "./logserver.py" ]
