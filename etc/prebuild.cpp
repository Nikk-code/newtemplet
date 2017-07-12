// ��������������, ��� � �������� utils ��������� ������� �� ������ newtemplet:
// skel -- ���������� �� newtemplet/dsl/skel.cpp
// gen  -- ���������� �� newtemplet/dsl/gen.cpp, newtemplet/dsl/lexer.cpp, newtemplet/dsl/parse.cpp
// dtaskbag.dsg.cpp -- ��� newtemplet\samples\dtaskbag\dtaskbag.dsg.cpp
// taskbag.dsg.cpp  -- ��� newtemplet\samples\taskbag\taskbag.dsg.cpp
// pipeline.dsg.cpp -- ��� newtemplet\samples\pipeline\pipeline.dsg.cpp
// dtaskbag.dpl.cpp -- ��� newtemplet\samples\dtaskbag\dtaskbag.dpl.cpp
// taskbag.dpl.cpp  -- ��� newtemplet\samples\taskbag\taskbag.dpl.cpp
// pipeline.dpl.cpp -- ��� newtemplet\samples\pipeline\pipeline.dpl.cpp


// ������� prebuild_design ������ ���� ����������� � ���� �������,
// ����������� � ��������� ����� prebuild_design.sh

void prebuild_design()
{
	for (auto file : "dir *.cpp") {// ��� ���� ������ � ����������� .cpp

		auto first_block_index = exec("utils/skel - i " + file); 
		    // ��������� ������� skel, ��� ������� �� ������� ������ ������� �����
		    // ���� ������ ��������� �����������, ����� ������ ������������
		    // ���� file -- ������� ����, skel ������� � ������� NOMARKUP

		switch (first_block_index) {
		
		case "actor" :
			
			cout << exec("utils/gen -design "+file+" "+file".dsg");
			// �� ���������� '#paragma templet' � ����� file ������������ ������, ����������� � ���� file.dsg

			cout << exec("utils/skel -i "+file+" -s "+file".dsg");
			// ������ �� file.dsg ����������� � ��������� ����� file (���� ��� ���� ����������)

			exec("del file" + ".dsg");
			// ���� ������� file.dsg ���������

			break;
		
		case "dtaskbag" :

			cout << exec("utils/skel -i " + file + " -s utils/dtaskbag.dsg.cpp");
			// ��������� � ����� file ������ '������������� �������� �����' �� ����� dtaskbag.dsg.cpp

			break;
		
		case "pipeline" :
			
			cout << exec("utils/skel -i " + file + " -s utils/pipeline.dsg.cpp");
			// ��������� � ����� file ������ '��������' �� ����� pipeline.dsg.cpp

			break;

		case "taskbag" :

			cout << exec("utils/skel -i " + file + " -s utils/taskbag.dsg.cpp");
			// ��������� � ����� file ������ '�������� ����� ��� ����������� ������' �� ����� taskbag.dsg.cpp
			break;
		
		case "NOMARKUP" :
			// ���������� ���� file
		}
	}
}

// ������� ���������� prebuild_design, ������ ������������ ������� � ����������� .dpl.cpp � ���� -deploy � ������� gen
void prebuild_deploy()
{
	for (auto file : "dir *.cpp") {

		auto first_block_index = exec("utils/skel - i " + file);

		switch (first_block_index) {

		case "actor":

			cout << exec("utils/gen -deploy " + file + " " + file".dpl");
			cout << exec("utils/skel -i " + file + " -s " + file".dpl");
			exec("del file" + ".dpl");

			break;

		case "dtaskbag":

			cout << exec("utils/skel -i " + file + " -s utils/dtaskbag.dpl.cpp");
			break;

		case "pipeline":

			cout << exec("utils/skel -i " + file + " -s utils/pipeline.dpl.cpp");
			break;

		case "taskbag":

			cout << exec("utils/skel -i " + file + " -s utils/taskbag.dpl.cpp");
			break;

		case "NOMARKUP":
			// ���������� ���� file
		}
	}
}