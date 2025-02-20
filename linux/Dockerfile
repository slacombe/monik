FROM alpine:latest
COPY ./monik-linux /usr/chess/monik
COPY ./book.bin /usr/chess/book.bin
COPY ./bookc.bin /usr/chess/bookc.bin
WORKDIR /usr/chess
CMD ["./monik", "-h", "8092"]
