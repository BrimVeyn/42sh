import React, { useEffect, useState } from 'react';
import Category from './Category'
import logo from './logo.svg';
import './App.css';

function App() {
	const [data, setData] = useState(null);

	useEffect(() => {
		// Fetch the JSON file from the public folder
		fetch('/log.json')
			.then((response) => {
				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`);
				}
				return response.json(); // Parse JSON
			})
			.then((json) => {
				setData(json); // Update state with parsed data
			})
			.catch((error) => {
				console.error('Error fetching the JSON file:', error);
			});
	}, []); // Empty dependency array to run only once on component mount

	console.log(data);

	return (
		<div className="App">
			<header className="App-header">
				{data ? (
					data.categories.map((category, index) => (
						<Category name={category.category_name} tests={category.tests} />
					))
				) : (
					<p> Loading test results </p>
				)}
			</header>
		</div>
	);
}

export default App;
