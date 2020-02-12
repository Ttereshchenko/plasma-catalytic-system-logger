FROM python:3.7.0-alpine3.7

COPY ./requriments.txt /

RUN pip install -r requriments.txt

COPY mqtt_client.py ./

CMD ["python", "mqtt_client.py"]
