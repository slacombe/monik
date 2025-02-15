FROM alpine:latest
COPY ./monik /usr/chess/monik
WORKDIR /usr/chess
CMD ["./monik"]
