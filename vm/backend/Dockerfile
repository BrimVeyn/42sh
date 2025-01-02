# Use the official Node.js image as the base image
FROM node:18-alpine

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy package.json and package-lock.json to the working directory
COPY package.json package-lock.json server.js ./

# Install dependencies
RUN npm install

EXPOSE 4000
# Command to start the Express backend
CMD ["node", "server.js"]
