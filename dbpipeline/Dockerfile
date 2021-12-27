FROM python:3.7.8

# Creating Application Source Code Directory
RUN mkdir -p /dbpipeline/src

# Copying src code to Container
COPY dbpipeline /dbpipeline/src
COPY requirements.txt /dbpipeline/src

# Setting Home Directory for containers
WORKDIR /dbpipeline/src

# Installing requirements
RUN pip install --no-cache-dir -r requirements.txt

CMD [ "python", "./dbpipeline.py", "batterymonitor" ]
