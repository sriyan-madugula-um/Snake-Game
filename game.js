const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

const box = 20; // Size of the grid squares
const canvasSize = 400; // Canvas size in pixels

let snake = [{ x: 200, y: 200 }];
let direction = 'RIGHT';
let food = {
    x: Math.floor(Math.random() * canvasSize / box) * box,
    y: Math.floor(Math.random() * canvasSize / box) * box
};

// Listen for keyboard events
document.addEventListener('keydown', changeDirection);

function changeDirection(event) {
    const key = event.keyCode;
    const goingUp = direction === 'UP';
    const goingDown = direction === 'DOWN';
    const goingRight = direction === 'RIGHT';
    const goingLeft = direction === 'LEFT';

    if ((key === 37 || key === 65) && !goingRight) { // Left arrow or 'A'
        direction = 'LEFT';
    }
    if ((key === 38 || key === 87) && !goingDown) { // Up arrow or 'W'
        direction = 'UP';
    }
    if ((key === 39 || key === 68) && !goingLeft) { // Right arrow or 'D'
        direction = 'RIGHT';
    }
    if ((key === 40 || key === 83) && !goingUp) { // Down arrow or 'S'
        direction = 'DOWN';
    }
}

function draw() {
    // Clear the canvas
    ctx.clearRect(0, 0, canvasSize, canvasSize);

    // Draw the snake
    for (let i = 0; i < snake.length; i++) {
        ctx.fillStyle = (i === 0) ? 'green' : 'white';
        ctx.fillRect(snake[i].x, snake[i].y, box, box);
        ctx.strokeStyle = 'red';
        ctx.strokeRect(snake[i].x, snake[i].y, box, box);
    }

    // Draw the food
    ctx.fillStyle = 'red';
    ctx.fillRect(food.x, food.y, box, box);

    // Move the snake
    let snakeX = snake[0].x;
    let snakeY = snake[0].y;

    if (direction === 'LEFT') snakeX -= box;
    if (direction === 'UP') snakeY -= box;
    if (direction === 'RIGHT') snakeX += box;
    if (direction === 'DOWN') snakeY += box;

    // Check for collision with the food
    if (snakeX === food.x && snakeY === food.y) {
        food = {
            x: Math.floor(Math.random() * canvasSize / box) * box,
            y: Math.floor(Math.random() * canvasSize / box) * box
        };
    } else {
        // Remove the tail
        snake.pop();
    }

    // Add the new head
    const newHead = { x: snakeX, y: snakeY };
    snake.unshift(newHead);

    // Check for collisions with the wall or itself
    if (snakeX < 0 || snakeX >= canvasSize || snakeY < 0 || snakeY >= canvasSize || collision(newHead, snake)) {
        clearInterval(game);
    }
}

function collision(head, array) {
    for (let i = 1; i < array.length; i++) {
        if (head.x === array[i].x && head.y === array[i].y) {
            return true;
        }
    }
    return false;
}

// Set the game loop
const game = setInterval(draw, 100);
