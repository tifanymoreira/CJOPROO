#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "MainWindow: setupUi concluído.";

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::on_btnAdd_clicked);
    connect(ui->btnUpdate, &QPushButton::clicked, this, &MainWindow::on_btnUpdate_clicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::on_btnDelete_clicked);

    connect(ui->txtTask, &QLineEdit::textChanged, this, &MainWindow::on_txtTask_textChanged);
    qDebug() << "MainWindow: Conexão txtTask_textChanged estabelecida.";

    ui->txtTask->setFocus();
    ui->btnAdd->setEnabled(false);
    qDebug() << "MainWindow: Botão 'Adicionar' inicialmente desabilitado.";


    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->primaryScreen()->geometry()));

    loadTasks();
}

MainWindow::~MainWindow()
{
    saveTasks();
    delete ui;
}

void MainWindow::clearInputField()
{
    ui->txtTask->clear();
    ui->txtTask->setFocus();
    qDebug() << "clearInputField: Campo de entrada limpo.";
}

void MainWindow::on_btnAdd_clicked()
{
    qDebug() << "on_btnAdd_clicked: Botão 'Adicionar' clicado.";
    QString taskText = ui->txtTask->text().trimmed();
    qDebug() << "on_btnAdd_clicked: Texto da tarefa (trim):" << taskText;

    if (taskText.isEmpty()) {
        qDebug() << "on_btnAdd_clicked: Aviso - Texto da tarefa está vazio. Exibindo QMessageBox.";
        QMessageBox::warning(this, "Aviso", "Por favor, digite uma tarefa.");
        return;
    }

    ui->listWidget->addItem(taskText);
    clearInputField();
    qDebug() << "on_btnAdd_clicked: Tarefa adicionada e campo limpo. Exibindo QMessageBox de sucesso."; // Mensagem de depuração
    QMessageBox::information(this, "Sucesso", "Tarefa adicionada com sucesso!");
}

void MainWindow::on_btnUpdate_clicked()
{
    qDebug() << "on_btnUpdate_clicked: Botão 'Atualizar' clicado.";
    QListWidgetItem *selectedItem = ui->listWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Aviso", "Selecione uma tarefa para atualizar.");
        return;
    }

    QString updatedText = ui->txtTask->text().trimmed();
    if (updatedText.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, digite o novo texto da tarefa.");
        return;
    }

    selectedItem->setText(updatedText);
    clearInputField();
    QMessageBox::information(this, "Sucesso", "Tarefa atualizada com sucesso!");
}

void MainWindow::on_btnDelete_clicked()
{
    qDebug() << "on_btnDelete_clicked: Botão 'Deletar' clicado.";
    QListWidgetItem *selectedItem = ui->listWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Aviso", "Selecione uma tarefa para excluir.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmar Exclusão",
                                  "Tem certeza que deseja excluir a tarefa selecionada?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    delete ui->listWidget->takeItem(ui->listWidget->row(selectedItem));

    clearInputField();
    QMessageBox::information(this, "Sucesso", "Tarefa excluída com sucesso!");
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    qDebug() << "on_listWidget_itemClicked: Item da lista clicado.";
    ui->txtTask->setText(item->text());
}

// Slot: Habilita/desabilita o botão 'Adicionar' baseado no texto do campo
void MainWindow::on_txtTask_textChanged(const QString &arg1)
{
    bool isEmpty = arg1.trimmed().isEmpty();
    ui->btnAdd->setEnabled(!isEmpty);
    qDebug() << "on_txtTask_textChanged: Texto: '" << arg1 << "', Está vazio (trim):" << isEmpty << ", Botão 'Adicionar' habilitado:" << !isEmpty; // Mensagem de depuração
}

void MainWindow::loadTasks()
{
    QFile file("tasks.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "loadTasks: Não foi possível abrir o arquivo tasks.txt para leitura.";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        ui->listWidget->addItem(line);
    }
    file.close();
    qDebug() << "loadTasks: Tarefas carregadas com sucesso!";
}

void MainWindow::saveTasks()
{
    QFile file("tasks.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "saveTasks: Não foi possível abrir o arquivo tasks.txt para escrita.";
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        out << ui->listWidget->item(i)->text() << "\n";
    }
    file.close();
    qDebug() << "saveTasks: Tarefas salvas com sucesso!";
}
