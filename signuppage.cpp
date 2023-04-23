#include "signuppage.h"

SignUpPage::SignUpPage()
{
    //Init stuff
    user = new User;
    firebaseStorage = new FirebaseStorage();
    firebaseSignIn = new FirebaseSignIn();
    dbHelper = new FirebaseDBHelper();
    firebaseAuth = new FirebaseAuth();
    firebaseAuth->setUser(user);
    firebaseSignIn->setUser(user);
    firebaseStorage->setUser(user);

    //Widget stuff
    QGridLayout *layout = new QGridLayout();
    QLabel *firstNameLabel = new QLabel("First Name: ");
    QLineEdit *qFirstNameEdit = new QLineEdit();
    qFirstNameEdit->setPlaceholderText("Enter first name");
    QLabel *lastNameLabel = new QLabel("Last Name: ");
    QLineEdit *qLastNameEdit = new QLineEdit();
    qLastNameEdit->setPlaceholderText("Enter last name");

    QLabel *usernameLabel = new QLabel("Username: ");
    QLineEdit *userName = new QLineEdit();
    userName->setPlaceholderText("Enter username");

    QLabel *warning = new QLabel();
    warning->hide();
    QLabel *passwordLabel = new QLabel("Password: ");
    QLineEdit *password = new QLineEdit();
    password->setEchoMode(QLineEdit::Password);
    password->setPlaceholderText("Enter password");
    QPushButton *showPasswordBtn = new QPushButton("Show Password");
    connect(showPasswordBtn, &QPushButton::released, [password]() {
        if (password->echoMode() == QLineEdit::Normal) {
            password->setEchoMode(QLineEdit::Password);
        } else {
            password->setEchoMode(QLineEdit::Normal);
        }
    });

    QLabel *birthdayLabel = new QLabel("Birthday: ");
    QDateEdit *birthday = new QDateEdit();
    birthday->setMinimumDate(QDate(1900, 1, 1));
    birthday->setMaximumDate(QDate::currentDate());
    birthday->setDisplayFormat("dd/MM/yyyy");

    profileLabel = new QLabel();
    QPushButton *loadFileBtn = new QPushButton("Pick Avatar");

    connect(loadFileBtn, &QPushButton::released, [this]() { loadProfilePic(); });

    QPushButton *saveToDatabase = new QPushButton("Save to Firebase");
    connect(saveToDatabase,
            &QPushButton::released,
            this,
            [qLastNameEdit, qFirstNameEdit, userName, password, birthday, this, warning, layout]() {
                QRegularExpression regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{8,}$");
                QRegularExpressionMatch match = regex.match(password->text());
                if (match.hasMatch()) {
                    user->username = userName->text().replace(" ", "");
                    user->firstName = qFirstNameEdit->text().replace(" ", "");
                    user->lastName = qLastNameEdit->text().replace(" ", "");
                    user->birthday = birthday->date().toString();
                    //Sign up first, then save the user info into realtime database
                    this->signUserUp(password->text().replace(" ", ""));
                } else {
                    warning->show();
                    layout->addWidget(warning, 1, 2);
                    warning->setText("Wrong password format");
                }
            });
    QPushButton *closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::released, this, &SignUpPage::closeAll);
    //    QPushButton *signIn = new QPushButton("Sign In");
    //    QObject::connect(signIn, &QPushButton::released, [&userName, &firebaseSignIn]() {
    //        //        user->username = userName->text();
    //        firebaseSignIn.signUserIn(userName->text() + "@gmail.com", "password123");
    //        FirebaseSignIn::connect(firebaseSignIn.networkReply,
    //                                &QNetworkReply::readyRead,
    //                                [&firebaseSignIn]() { firebaseSignIn.networkReplyResponse(); });
    //    });

    layout->addWidget(usernameLabel, 0, 0);
    layout->addWidget(userName, 0, 1);
    layout->addWidget(passwordLabel, 1, 0);
    layout->addWidget(password, 1, 1);

    layout->addWidget(firstNameLabel, 2, 0);
    layout->addWidget(qFirstNameEdit, 2, 1);
    layout->addWidget(lastNameLabel, 3, 0);
    layout->addWidget(qLastNameEdit, 3, 1);

    layout->addWidget(birthdayLabel, 4, 0);
    layout->addWidget(birthday, 4, 1);

    layout->addWidget(profileLabel, 5, 0);
    layout->addWidget(loadFileBtn, 5, 1);

    layout->addWidget(saveToDatabase, 6, 0);
    layout->addWidget(showPasswordBtn, 6, 1);
    layout->addWidget(closeBtn, 6, 2);
    //    layout->addWidget(signIn);
    this->setLayout(layout);
    this->setFixedSize(600, 300);
}

void SignUpPage::loadProfilePic()
{
    QString defaultPath = QDir::homePath();
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     "Open a file",
                                                     defaultPath,
                                                     "Images (*.png *.jpg)");
    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
    } else {
        profileLabel->setText("Upload Success!");
    }
    fileData = file.readAll();
    QFileInfo fileInfo(file);
    fileType = fileInfo.completeSuffix();
    file.close();
}

void SignUpPage::signUserUp(QString password)
{
    qDebug() << "Sign user up";
    firebaseAuth->signUserUp(user->username + "@gmail.com", password);

    FirebaseAuth::connect(firebaseAuth->networkAccessManager,
                          &QNetworkAccessManager::finished,
                          this,
                          &SignUpPage::uploadToStorage);
}

void SignUpPage::uploadToStorage()
{
    qDebug() << "Finished Sign user up";
    qDebug() << "Upload to storage";
    firebaseStorage->uploadToStorage(fileData, fileType);

    FirebaseStorage::connect(firebaseStorage->networkManager,
                             &QNetworkAccessManager::finished,
                             this,
                             &SignUpPage::uploadToFirebase);
}

void SignUpPage::uploadToFirebase()
{
    qDebug() << "Finished upload to storage";
    qDebug() << "Upload to database";
    dbHelper->uploadToDatabase(user);
    qDebug() << "Finished upload to database";

    connect(dbHelper->getNetworkManager(),
            &QNetworkAccessManager::finished,
            this,
            &SignUpPage::closeAll);
}

void SignUpPage::closeAll()
{
    this->close();
}