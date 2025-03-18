#include <string>
#include <mysql/mysql.h>
using namespace std;

class Empleado {
public:
    int id;
    string nombreCompleto, puesto, curp, direccion, empresaAsignada;
    string tipoContrato, numeroCredencial, estado, fechaIngreso;

    void registrar(MYSQL* conn);
    void editar(MYSQL* conn);
    void eliminar(MYSQL* conn);
    static void buscar(MYSQL* conn);
};

class ConexionDB {
public:
    static MYSQL* conectar();
};

MYSQL* ConexionDB::conectar() {
    MYSQL* conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "usuario", "password", "basedatos", 3306, NULL, 0);
    
    if (conn) {
        cout << "Conexion exitosa" << endl;
    } else {
        cerr << "Error al conectar: " << mysql_error(conn) << endl;
    }
    return conn;
}

void Empleado::registrar(MYSQL* conn) {
    cout << "\n--- Registrar Nuevo Empleado ---\n";
    cout << "ID: "; cin >> id;
    cin.ignore();
    cout << "Nombre Completo: "; getline(cin, nombreCompleto);
    cout << "Puesto: "; getline(cin, puesto);
    cout << "CURP: "; getline(cin, curp);
    cout << "Direccion: "; getline(cin, direccion);
    cout << "Empresa Asignada: "; getline(cin, empresaAsignada);
    cout << "Tipo de Contrato: "; getline(cin, tipoContrato);
    cout << "Numero de Credencial: "; getline(cin, numeroCredencial);
    cout << "Estado: "; getline(cin, estado);
    cout << "Fecha de Ingreso (YYYY-MM-DD): "; getline(cin, fechaIngreso);
    
    string query = "INSERT INTO empleados (id, nombreCompleto, puesto, curp, direccion, empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) VALUES (" +
        to_string(id) + ", '" + nombreCompleto + "', '" + puesto + "', '" + curp + "', '" + direccion + "', '" + empresaAsignada + "', '" + tipoContrato + "', '" + numeroCredencial + "', '" + estado + "', '" + fechaIngreso + "')";
    
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado registrado correctamente." << endl;
    } else {
        cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::editar(MYSQL* conn) {
    cout << "\nIngrese ID del empleado a modificar: ";
    cin >> id;
    cin.ignore();
    cout << "Nuevo Nombre Completo: "; getline(cin, nombreCompleto);
    
    string query = "UPDATE empleados SET nombreCompleto='" + nombreCompleto + "' WHERE id=" + to_string(id);
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado actualizado correctamente." << endl;
    } else {
        cerr << "Error al actualizar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::eliminar(MYSQL* conn) {
    cout << "\nIngrese ID del empleado a eliminar: ";
    cin >> id;
    string query = "DELETE FROM empleados WHERE id=" + to_string(id);
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado eliminado correctamente." << endl;
    } else {
        cerr << "Error al eliminar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::buscar(MYSQL* conn) {
    int opcion;
    cout << "\nBuscar por: 1. ID 2. Nombre\nOpcion: ";
    cin >> opcion;
    cin.ignore();
    string query;
    if (opcion == 1) {
        int id;
        cout << "Ingrese ID: ";
        cin >> id;
        query = "SELECT * FROM empleados WHERE id=" + to_string(id);
    } else {
        string nombre;
        cout << "Ingrese Nombre: ";
        getline(cin, nombre);
        query = "SELECT * FROM empleados WHERE nombreCompleto LIKE '%" + nombre + "%'";
    }
    
    if (mysql_query(conn, query.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << " | Nombre: " << row[1] << " | Puesto: " << row[2] << endl;
        }
        mysql_free_result(res);
    } else {
        cerr << "Error en la búsqueda: " << mysql_error(conn) << endl;
    }
}

void menuPrincipal(MYSQL* conn) {
    int opcion;
    Empleado emp;
    do {
        cout << "\n1. Registrar Empleado\n2. Editar Empleado\n3. Eliminar Empleado\n4. Buscar Empleado\n5. Salir\nSeleccione una opcion: ";
        cin >> opcion;
        cin.ignore();
        switch (opcion) {
            case 1: emp.registrar(conn); break;
            case 2: emp.editar(conn); break;
            case 3: emp.eliminar(conn); break;
            case 4: Empleado::buscar(conn); break;
            case 5: cout << "Saliendo..." << endl; break;
            default: cout << "Opcion invalida." << endl;
        }
    } while (opcion != 5);
}

int main() {
    MYSQL* conn = ConexionDB::conectar();
    if (conn) {
        menuPrincipal(conn);
        mysql_close(conn);
    }
    return 0;
}
