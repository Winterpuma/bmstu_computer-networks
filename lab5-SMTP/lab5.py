from os.path import isfile, basename, splitext
from os import listdir
import smtplib
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


# Отправка текстового письма
def send_simple_mail(from_address, to_address, password, message):
    server = smtplib.SMTP('smtp.mail.ru', 25)
    server.starttls()
    server.login(from_address, password)
    server.sendmail(from_address, to_address, message)
    server.quit()


# Поиск файлов содержащих ключевое слово в текущей директории
def find_files_with_keyword(keyword):
    if keyword == '':
        return

    files_with_keyword = []

    for filename in listdir("."):
        if not isfile(filename) or splitext(filename)[1].lower() != '.txt':
            continue
        with open(filename) as file:
            file_data = file.read()
            if keyword in file_data:
                files_with_keyword.append(filename)

    return files_with_keyword


# Прикрепление файлов к письму
def attach_files_to_mime(mime, key_files):
    for filename in key_files:
        with open(filename, 'rb') as file:
            part = MIMEApplication(file.read(), Name=basename(filename))
        part['Content-Disposition'] = 'attachment; filename="%s"' % basename(filename)
        mime.attach(part)


# Заполнение полей письма
def fill_mime(from_address, to_address, msg_subject, msg_text, filenames):
    mime = MIMEMultipart()
    mime['From'] = from_address
    mime['To'] = to_address
    mime['Subject'] = msg_subject
    mime.attach(MIMEText(msg_text, 'plain'))
    attach_files_to_mime(mime, filenames)
    return mime


# Отправка письма с вложениями
def send_mime_mail(mime, smtp_host, from_address, password):
    server = smtplib.SMTP(smtp_host[0], smtp_host[1])
    server.starttls()
    server.login(from_address, password)
    server.sendmail(mime['From'], mime['To'], mime.as_string())
    server.quit()


FROM = 'example@mail.ru'
TO = 'example@mail.ru'
PAS = ''
SUBJ = 'Msg from python'
MSG = 'Hello there'
KEY = 'key'
SMTP_HOST = ["smtp.mail.ru", 25]


def input_data():
    global TO, FROM, PAS, SUBJ, MSG, KEY
    TO = input("Enter receiver email address: ")
    FROM = input("Enter sender email address: ")
    PAS = input("Enter sender password: ")
    SUBJ = input("Enter mail subject: ")
    MSG = input("Enter mail message")
    KEY = input("Enter keyword: ")


def main():
    key_filenames = find_files_with_keyword(KEY)
    mime = fill_mime(FROM, TO, SUBJ, MSG, key_filenames)
    send_mime_mail(mime, SMTP_HOST, FROM, PAS)


input_data()
main()
