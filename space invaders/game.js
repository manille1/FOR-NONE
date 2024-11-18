const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

let player = { x: canvas.width / 2, y: canvas.height - 30, width: 50, height: 50, speed: 5, dx: 0 };
let invaders = [];
let bullets = [];
let score = 0;
let gameInterval;
let isGameOver = false;

function createInvaders() {
    let invaderWidth = 40;
    let invaderHeight = 30;
    for (let row = 0; row < 5; row++) {
        for (let col = 0; col < 10; col++) {
            invaders.push({ x: col * (invaderWidth + 10), y: row * (invaderHeight + 10), width: invaderWidth, height: invaderHeight });
        }
    }
}

function drawPlayer() {
    ctx.fillStyle = 'green';
    ctx.fillRect(player.x, player.y, player.width, player.height);
}

function drawInvaders() {
    ctx.fillStyle = 'red';
    invaders.forEach(invader => {
        ctx.fillRect(invader.x, invader.y, invader.width, invader.height);
    });
}

function drawBullets() {
    ctx.fillStyle = 'yellow';
    bullets.forEach(bullet => {
        ctx.fillRect(bullet.x, bullet.y, bullet.width, bullet.height);
    });
}

function movePlayer() {
    player.x += player.dx;
    if (player.x < 0) player.x = 0;
    if (player.x + player.width > canvas.width) player.x = canvas.width - player.width;
}

function moveBullets() {
    for (let i = 0; i < bullets.length; i++) {
        bullets[i].y -= 5;
        if (bullets[i].y < 0) bullets.splice(i, 1); // Remove bullet when it goes off-screen
    }
}

function checkCollisions() {
    for (let i = 0; i < bullets.length; i++) {
        for (let j = 0; j < invaders.length; j++) {
            let bullet = bullets[i];
            let invader = invaders[j];

            if (bullet.x < invader.x + invader.width &&
                bullet.x + bullet.width > invader.x &&
                bullet.y < invader.y + invader.height &&
                bullet.y + bullet.height > invader.y) {
                // Collision detected
                invaders.splice(j, 1);
                bullets.splice(i, 1);
                score += 10;
                break;
            }
        }
    }
}

function updateScore() {
    document.getElementById('score').textContent = score;
}

function draw() {
    if (isGameOver) {
        clearInterval(gameInterval);
        alert('Game Over! Your score is: ' + score);
        document.getElementById('restartButton').style.display = 'inline-block';
        return;
    }

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawPlayer();
    drawInvaders();
    drawBullets();
    movePlayer();
    moveBullets();
    checkCollisions();
    updateScore();
}

function startGame() {
    createInvaders();
    gameInterval = setInterval(draw, 1000 / 60); // 60 FPS
}

function restartGame() {
    invaders = [];
    bullets = [];
    score = 0;
    player.x = canvas.width / 2;
    isGameOver = false;
    document.getElementById('restartButton').style.display = 'none';
    startGame();
}

// Key event listeners
document.addEventListener('keydown', (e) => {
    if (e.key === 'ArrowLeft') player.dx = -player.speed;
    if (e.key === 'ArrowRight') player.dx = player.speed;
    if (e.key === ' ') {
        bullets.push({ x: player.x + player.width / 2 - 2, y: player.y, width: 4, height: 10 });
    }
});

document.addEventListener('keyup', (e) => {
    if (e.key === 'ArrowLeft' || e.key === 'ArrowRight') player.dx = 0;
});

document.getElementById('restartButton').addEventListener('click', restartGame);

startGame();