FROM alpine:latest
COPY ./monik-linux /usr/chess/monik
COPY ./book.bin /usr/chess/book.bin
WORKDIR /usr/chess
CMD ["./monik", "-h", "8092"]
