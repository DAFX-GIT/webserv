#!/usr/bin/env python3

import os
import sys
import random
import json
from urllib.parse import parse_qs

# Ensure the browser knows we are sending HTML back
print("Content-Type: text/html\r\n\r\n", end="")

# A small dictionary of possible secret words and valid guesses
WORD_BANK = [
    "APPLE", "BEACH", "CHIEF", "DRIVE", "EAGLE",
    "FLAME", "GUIDE", "HOUSE", "INDEX", "JUDGE",
    "KNIFE", "LIGHT", "MATCH", "NIGHT", "OCEAN",
    "PAPER", "QUEEN", "RADIO", "SNAKE", "TRAIN",
    "UNDER", "VOICE", "WATER", "YOUTH", "ZEBRA"
]

# Read form data (GET or POST)
method = os.environ.get("REQUEST_METHOD", "GET")

if method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    raw_data = sys.stdin.read(content_length)
else:
    raw_data = os.environ.get("QUERY_STRING", "")

form = parse_qs(raw_data)

def getvalue(name, default=None):
    values = form.get(name)
    return values[0] if values else default

# Retrieve existing game state, or initialize a new game
secret_word = getvalue("secret_word")
guesses_json = getvalue("guesses")

if secret_word is None:
    # No game in progress, set up a new one
    secret_word = random.choice(WORD_BANK)
    guesses = []
else:
    try:
        guesses = json.loads(guesses_json)
    except (TypeError, json.JSONDecodeError):
        guesses = []

message = "Enter a 5-letter word to start guessing!"
game_over = False
won = False

# Process a new guess if submitted
new_guess = getvalue("guess")

if new_guess and not game_over:
    new_guess = new_guess.upper().strip()

    if len(new_guess) != 5:
        message = "⚠️ Word must be exactly 5 letters long!"
    else:
        guesses.append(new_guess)

        if new_guess == secret_word:
            message = "🎉 Genius! You guessed the word!"
            won = True
            game_over = True
        elif len(guesses) >= 6:
            message = f"💥 Game Over! The word was <strong>{secret_word}</strong>."
            game_over = True
        else:
            message = f"Keep going! Guess {len(guesses) + 1} of 6."

# Build the Wordle grid
grid_html = ""

for i in range(6):
    grid_html += '<div class="wordle-row">'

    if i < len(guesses):
        current_guess = guesses[i]

        for index, letter in enumerate(current_guess):
            if secret_word[index] == letter:
                status_class = "correct"
            elif letter in secret_word:
                status_class = "present"
            else:
                status_class = "absent"

            grid_html += (
                f'<div class="wordle-cell {status_class}">{letter}</div>'
            )
    else:
        for _ in range(5):
            grid_html += '<div class="wordle-cell"></div>'

    grid_html += '</div>'

# HTML Layout and CSS styling
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Wordle</title>

    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #121213;
            color: white;
            text-align: center;
            margin-top: 30px;
        }}

        .container {{
            max-width: 500px;
            margin: 0 auto;
            padding: 20px;
        }}

        h1 {{
            letter-spacing: 2px;
            border-bottom: 1px solid #3a3a3c;
            padding-bottom: 10px;
        }}

        .message {{
            font-size: 1.1rem;
            margin: 15px 0;
            color: #d7dedc;
        }}

        .wordle-grid {{
            display: flex;
            flex-direction: column;
            gap: 5px;
            align-items: center;
            margin-bottom: 25px;
        }}

        .wordle-row {{
            display: flex;
            gap: 5px;
        }}

        .wordle-cell {{
            width: 50px;
            height: 50px;
            border: 2px solid #3a3a3c;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.8rem;
            font-weight: bold;
            text-transform: uppercase;
        }}

        .correct {{
            background-color: #538d4e;
            border-color: #538d4e;
        }}

        .present {{
            background-color: #b59f3b;
            border-color: #b59f3b;
        }}

        .absent {{
            background-color: #3a3a3c;
            border-color: #3a3a3c;
        }}

        input[type="text"] {{
            padding: 10px;
            font-size: 1.2rem;
            width: 150px;
            text-align: center;
            text-transform: uppercase;
            background-color: #121213;
            border: 2px solid #565758;
            color: white;
            border-radius: 4px;
        }}

        input[type="submit"],
        .btn-reset {{
            padding: 10px 20px;
            font-size: 1.2rem;
            background-color: #538d4e;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            margin-left: 5px;
        }}

        .btn-reset {{
            background-color: #007bff;
            text-decoration: none;
            display: inline-block;
        }}

        .btn-reset:hover,
        input[type="submit"]:hover {{
            opacity: 0.9;
        }}
    </style>
</head>

<body>
    <div class="container">
        <h1>WORDLE (CGI)</h1>

        <p class="message">{message}</p>

        <div class="wordle-grid">
            {grid_html}
        </div>

        {
            "<a href='wordle.py' class='btn-reset'>Play Again</a>"
            if game_over
            else f'''
            <form method="POST" action="wordle.py">
                <input
                    type="hidden"
                    name="secret_word"
                    value="{secret_word}"
                >

                <input
                    type="hidden"
                    name="guesses"
                    value='{json.dumps(guesses)}'
                >

                <input
                    type="text"
                    name="guess"
                    maxlength="5"
                    minlength="5"
                    pattern="[A-Za-z]{{5}}"
                    required
                    autofocus
                    autocomplete="off"
                >

                <input type="submit" value="Enter">
            </form>
            '''
        }
    </div>
</body>
</html>
"""

print(html_content)