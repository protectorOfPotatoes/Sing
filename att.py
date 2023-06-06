import speech_recognition as sr

recognizer = sr.Recognizer()

with sr.Microphone() as mic:
    print("Speak something...")
    audio = recognizer.listen(mic)

    try:
        text = recognizer.recognize_google(audio)
        print("Recognized:", text)

        with open("audio.txt", "w") as file:
            file.write(text)

        print("Text written to 'audio.txt' successfully.")

    except sr.UnknownValueError:
        print("Speech recognition could not understand audio.")

    except sr.RequestError as e:
        print("Could not request results from Google Speech Recognition service; {0}".format(e))

