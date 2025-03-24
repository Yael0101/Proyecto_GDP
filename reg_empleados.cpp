#include <iostream>
#include <string>
#include <mysql/mysql.h>

using namespace std;

class Empleado {
public:
    int id;
    string nombreCompleto, puesto, curp, direccion, empresaAsignada;
    string tipoContrato, numeroCredencial, estado, fechaIngreso;

    void registrar(MYSQL* conn);
    bool existeEmpleado(MYSQL* conn, int id, const string& curp);
};

class ConexionDB {
public:
    static MYSQL* conectar();
};

// Establece la conexión con la base de datos
MYSQL* ConexionDB::conectar() {
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) {
        cerr << "Error al inicializar MySQL." << endl;
        return nullptr;
    }

    conn = mysql_real_connect(conn, "localhost", "usuario", "password", "basedatos", 3306, NULL, 0);
    if (!conn) {
        cerr << "Error al conectar: " << mysql_error(conn) << endl;
    }
    return conn;
}

// Verifica si un empleado ya existe en la base de datos
bool Empleado::existeEmpleado(MYSQL* conn, int id, const string& curp) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return false;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Error al inicializar statement: " << mysql_error(conn) << endl;
        return false;
    }

    const char* query = "SELECT COUNT(*) FROM empleados WHERE id = ? OR curp = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Error al preparar statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[2] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &id;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)curp.c_str();
    bind[1].buffer_length = curp.length();
    bind[1].length = nullptr;
    bind[1].is_null = 0;

    mysql_stmt_bind_param(stmt, bind);
    mysql_stmt_execute(stmt);

    MYSQL_BIND result_bind = {};
    int count = 0;
    result_bind.buffer_type = MYSQL_TYPE_LONG;
    result_bind.buffer = &count;
    mysql_stmt_bind_result(stmt, &result_bind);

    if (mysql_stmt_fetch(stmt) == MYSQL_NO_DATA) {
        count = 0;
    }

    mysql_stmt_close(stmt);
    return count > 0;
}

// Registra un nuevo empleado en la base de datos
void Empleado::registrar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Registrar Nuevo Empleado ---\n";
    cout << "ID: ";
    string idStr;
    getline(cin, idStr);
    id = stoi(idStr);

    cout << "CURP: ";
    getline(cin, curp);
    if (curp.length() != 18) {
        cerr << "Error: CURP inválido. Debe tener 18 caracteres." << endl;
        return;
    }

    if (existeEmpleado(conn, id, curp)) {
        cerr << "Error: El empleado con ese ID o CURP ya está registrado." << endl;
        return;
    }

    cout << "Nombre Completo: "; getline(cin, nombreCompleto);
    cout << "Puesto: "; getline(cin, puesto);
    cout << "Direccion: "; getline(cin, direccion);
    cout << "Empresa Asignada: "; getline(cin, empresaAsignada);
    cout << "Tipo de Contrato: "; getline(cin, tipoContrato);
    cout << "Numero de Credencial: "; getline(cin, numeroCredencial);
    cout << "Estado: "; getline(cin, estado);
    cout << "Fecha de Ingreso (YYYY-MM-DD): "; getline(cin, fechaIngreso);

    if (fechaIngreso.length() != 10 || fechaIngreso[4] != '-' || fechaIngreso[7] != '-') {
        cerr << "Error: Fecha de ingreso inválida. Formato correcto: YYYY-MM-DD." << endl;
        return;
    }

    string query = "INSERT INTO empleados (id, nombreCompleto, puesto, curp, direccion, empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Error al inicializar statement: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length()) != 0) {
        cerr << "Error al preparar statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND bind[10] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &id;

    string* values[] = { &nombreCompleto, &puesto, &curp, &direccion, &empresaAsignada, &tipoContrato, &numeroCredencial, &estado, &fechaIngreso };
    for (int i = 1; i < 10; i++) {
        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = (char*)values[i - 1]->c_str();
        bind[i].buffer_length = values[i - 1]->length();
        bind[i].length = nullptr;
        bind[i].is_null = 0;
    }

    mysql_stmt_bind_param(stmt, bind);
    if (mysql_stmt_execute(stmt) == 0) {
        cout << "Empleado registrado correctamente." << endl;
    }
    else {
        cerr << "Error al registrar empleado: " << mysql_stmt_error(stmt) << endl;
    }

    mysql_stmt_close(stmt);
}
