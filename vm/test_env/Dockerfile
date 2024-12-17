FROM node:20

# Set the working directory
WORKDIR /app

# Install dependencies
COPY package.json package-lock.json ./
RUN npm install

# Copy the scheduler code and the script
COPY . .

# Make sure the script is executable
RUN chmod +x tester.sh

# Expose WebSocket port
EXPOSE 4000

# Run the scheduler
CMD ["node", "server.js"]
