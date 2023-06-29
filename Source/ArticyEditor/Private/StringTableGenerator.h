//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

/**
 * Holds a content string which can be written to a file, specified in the constructor.
 */
// TODO: Share common code with CodeFileGenerator due to similarities
class StringTableGenerator
{
public:

	/**
	 * Executes a lambda. If the Lambda is nullptr (type nullptr_t), nothing bad happens.
	 */
	template<typename Lambda>
	void SafeExecute(Lambda Lamb);

	/**
	 * Creates a new string table generator then executes the ContentGenerator.
	 * At last, WriteToFile is called.
	 */
	template<typename Lambda>
	StringTableGenerator(const FString& Culture, Lambda ContentGenerator);

	/** Add a line to the content. */
	void Line(const FString& Key = "", const FString& SourceString = "");

private:

	FString Path;
	FString FileContent = "";

	/** Write the content to file. */
	void WriteToFile() const;
};

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

template<typename Lambda>
void StringTableGenerator::SafeExecute(Lambda Lamb)
{
	Lamb();
}
template<>
inline void StringTableGenerator::SafeExecute(nullptr_t Lamb)
{
}

template <typename Lambda>
StringTableGenerator::StringTableGenerator(const FString& Culture, Lambda ContentGenerator)
{
	Path = FPaths::ProjectContentDir() / TEXT("ArticyContent/Generated/ArticyStrings");
	if (!Culture.IsEmpty())
	{
		Path += TEXT("-") + Culture;
	}
	Path += TEXT(".csv");
	
	Line("Key,SourceString");
	if(ensure(!std::is_null_pointer<Lambda>::value))
		ContentGenerator(this);

	WriteToFile();
}
