all: build

build:
	docker-compose up --build -d

stop:
	docker-compose down

ps:
	docker-compose ps

clean:
	docker rm -f $(docker ps -aq)
	docker rmi -f $(docker images -q)
	docker volume rm $(docker volume ls -q)
	docker network rm $(docker network ls -q)
