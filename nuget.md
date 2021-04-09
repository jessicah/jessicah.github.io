Publishing Packages to GitHub
=============================

1. `dotnet new nuget-configfile`
2. `dotnet nuget add source "https://nuget.pkg.github.com/jessicah/index.json" --name "github" --username "jessicah" --password <github api key>`
3. `dotnet pack --configuration Release`
4. `dotnet nuget push .\path\to\package.nupkg --source github`

But in addition to that, you also need to set up your project properties, under `Package`:

Particularly: Authors/Company, version numbers, and Repository URL

I haven't figured out if there's a nicer way to store the api key, although it does appear to be per-machine encrypted at least.
